//
// Created by yuri on 2026/7/13.
//

export module yuri.ui.widgets:context_menu;

import :base;
import yuri.core;
import yuri.skia;
import yuri.ui.render;
import yuri.ui.layout;
import std;

using namespace skia;
using namespace ui::layout;
using namespace ui::render;

namespace {

constexpr float kDefaultMenuWidth = 156.0f;                         // 默认菜单宽度
constexpr float kOuterPadding = 8.0f;                               // 菜单外层鼠标缓冲区
constexpr float kMenuPadding = 6.0f;                                // 菜单内边距
constexpr float kMenuItemHeight = 36.0f;                            // 菜单项高度
constexpr float kMenuRadius = 12.0f;                                // 菜单圆角
constexpr SkColor kMenuBgColor = ColorFromARGB(250, 255, 255, 255); // 菜单背景色
constexpr SkColor kMenuBorderColor = ColorFromARGB(32, 24, 31, 42); // 菜单描边色
constexpr SkColor kMenuHoverColor = ColorFromARGB(24, 218, 52, 92); // 菜单项悬浮色
constexpr SkColor kMenuTextColor = ColorFromARGB(255, 35, 43, 56);  // 菜单文字色

} // namespace

export namespace ui::widgets {

/** 上下文菜单项 */
class ContextMenuItem {
public:
  /** 创建上下文菜单项 */
  explicit ContextMenuItem(std::string name);

  /** 获取菜单项名称 */
  [[nodiscard]] const std::string &name() const noexcept;

private:
  std::string name_{}; // 菜单项名称
};

/** 通用弹出式上下文菜单 */
class ContextMenu final : public Widget {
public:
  /** 创建上下文菜单 */
  explicit ContextMenu(std::vector<ContextMenuItem> items, Widget *anchor);

  /** 销毁上下文菜单 */
  ~ContextMenu() override;

  /** 更新菜单项 */
  void setItems(std::vector<ContextMenuItem> items);

  /** 在鼠标当前位置弹出菜单 */
  void popupAtCursor();

  /** 在窗口指定位置弹出菜单 */
  void popup(float x, float y);

  /** 隐藏菜单 */
  void hide();

  /** 隐藏当前处于活动状态的上下文菜单 */
  static void hideActive();

  /** 绘制上下文菜单 */
  void paint(SkCanvas *canvas) override;

  /** 菜单项点击事件 */
  Signal<std::size_t> itemClicked{};

protected:
  /** 更新菜单项悬浮状态 */
  void onMouseMove(float x, float y) override;

  /** 清除菜单项悬浮状态 */
  void onMouseLeave(float x, float y) override;

  /** 处理菜单项点击 */
  void onMouseLeftReleased(float x, float y) override;

private:
  /** 根据坐标获取菜单项索引 */
  [[nodiscard]] int itemIndexAt(float x, float y) const noexcept;

  /** 获取菜单当前高度 */
  [[nodiscard]] float menuHeight() const noexcept;

  inline static ContextMenu *active_menu = nullptr; // 当前活动菜单
  std::vector<ContextMenuItem> items_{};            // 菜单项
  std::vector<RenderText> item_texts{};             // 菜单项渲染节点
  float menu_width = kDefaultMenuWidth;             // 菜单宽度
  int hovered_item = -1;                            // 当前悬浮菜单项
};

ContextMenuItem::ContextMenuItem(std::string name) : name_(std::move(name)) {
}

const std::string &ContextMenuItem::name() const noexcept {
  return name_;
}

ContextMenu::ContextMenu(std::vector<ContextMenuItem> items, Widget *anchor) :
  Widget(anchor == nullptr ? nullptr : anchor->rootWidget()) {
  setPadding(kOuterPadding);
  setItems(std::move(items));
  setVisible(false);
}

ContextMenu::~ContextMenu() {
  if (active_menu == this) {
    active_menu = nullptr;
  }
}

void ContextMenu::setItems(std::vector<ContextMenuItem> items) {
  items_ = std::move(items);
  item_texts.clear();
  item_texts.reserve(items_.size());

  for (std::size_t index = 0; index < items_.size(); ++index) {
    auto &text = item_texts.emplace_back(items_[index].name());
    text.setFontSize(13.5f);
    text.setColor(kMenuTextColor);
    text.setAlignment(Alignment::CenterLeft);
    text.update(
      SkRect::MakeXYWH(
        padding_.left + 18.0f,
        padding_.top + kMenuPadding + static_cast<float>(index) * kMenuItemHeight,
        menu_width - 36.0f,
        kMenuItemHeight
      )
    );
  }

  setGeometry(
    x(),
    y(),
    menu_width + padding_.left + padding_.right,
    menuHeight() + padding_.top + padding_.bottom
  );
  if (items_.empty()) {
    hide();
  }
}

void ContextMenu::popupAtCursor() {
  const auto cursor_position = window()->cursorPosition();
  popup(cursor_position.x(), cursor_position.y());
}

void ContextMenu::popup(const float x, const float y) {
  if (items_.empty() || parent_ == nullptr) {
    return;
  }

  if (active_menu != nullptr && active_menu != this) {
    active_menu->hide();
  }

  const float menu_x = std::clamp(x, 0.0f, std::max(0.0f, parent_->width() - menu_width));
  float menu_y = y;
  if (menu_y + menuHeight() > parent_->height()) {
    menu_y = y - menuHeight() + 1.0f;
  }
  menu_y = std::clamp(menu_y, 0.0f, std::max(0.0f, parent_->height() - menuHeight()));

  move(menu_x - padding_.left, menu_y - padding_.top);
  setVisible(true);
  hovered_ = true;
  active_menu = this;
}

void ContextMenu::hide() {
  setVisible(false);
  hovered_ = false;
  hovered_item = -1;
  window()->setCursor(CursorType::Arrow);
  if (active_menu == this) {
    active_menu = nullptr;
  }
}

void ContextMenu::hideActive() {
  if (active_menu != nullptr) {
    active_menu->hide();
  }
}

void ContextMenu::paint(SkCanvas *canvas) {
  const auto menu_rect = SkRect::MakeXYWH(padding_.left, padding_.top, menu_width, menuHeight());

  // 柔和阴影用于将菜单从下层内容中分离
  SkPaint shadow_layer;
  shadow_layer.setImageFilter(SkImageFilters::Blur(8.0f, 8.0f, nullptr));
  SkPaint shadow;
  shadow.setAntiAlias(true);
  shadow.setColor(ColorFromARGB(42, 15, 23, 42));
  const auto shadow_bounds = menu_rect.makeOutset(24.0f, 24.0f);
  canvas->saveLayer(&shadow_bounds, &shadow_layer);
  canvas->drawRoundRect(menu_rect.makeOffset(0.0f, 4.0f), kMenuRadius, kMenuRadius, shadow);
  canvas->restore();

  SkPaint background;
  background.setAntiAlias(true);
  background.setColor(kMenuBgColor);
  canvas->drawRoundRect(menu_rect, kMenuRadius, kMenuRadius, background);

  // ReSharper disable once CppDFAConstantConditions
  if (hovered_item >= 0) {
    // ReSharper disable once CppDFAUnreachableCode
    SkPaint hover;
    hover.setAntiAlias(true);
    hover.setColor(kMenuHoverColor);
    const float hover_y =
      padding_.top + kMenuPadding + static_cast<float>(hovered_item) * kMenuItemHeight;
    canvas->drawRoundRect(
      SkRect::MakeXYWH(
        padding_.left + kMenuPadding, hover_y, menu_width - kMenuPadding * 2.0f, kMenuItemHeight
      ),
      8.0f,
      8.0f,
      hover
    );
  }

  SkPaint border;
  border.setAntiAlias(true);
  border.setStyle(SkPaint::kStroke_Style);
  border.setStrokeWidth(1.0f);
  border.setColor(kMenuBorderColor);
  canvas->drawRoundRect(menu_rect.makeInset(0.5f, 0.5f), kMenuRadius, kMenuRadius, border);

  for (auto &text : item_texts) {
    text.render(canvas);
  }
}

void ContextMenu::onMouseMove(const float x, const float y) {
  hovered_item = itemIndexAt(x, y);
  window()->setCursor(hovered_item >= 0 ? CursorType::Hand : CursorType::Arrow);
}

void ContextMenu::onMouseLeave(float, float) {
  hide();
}

void ContextMenu::onMouseLeftReleased(const float x, const float y) {
  const int clicked_item = itemIndexAt(x, y);
  hide();
  if (clicked_item >= 0) {
    itemClicked.emit(static_cast<std::size_t>(clicked_item));
  }
}

int ContextMenu::itemIndexAt(const float x, const float y) const noexcept {
  const float content_x = x - padding_.left;
  const float content_y = y - padding_.top;
  if (
    content_x < kMenuPadding || content_x >= menu_width - kMenuPadding || content_y < kMenuPadding
    || content_y >= menuHeight() - kMenuPadding
  ) {
    return -1;
  }

  const auto index = static_cast<std::size_t>((content_y - kMenuPadding) / kMenuItemHeight);
  return index < items_.size() ? static_cast<int>(index) : -1;
}

float ContextMenu::menuHeight() const noexcept {
  return kMenuPadding * 2.0f + static_cast<float>(items_.size()) * kMenuItemHeight;
}

} // namespace ui::widgets
