//
// Created by yuri on 2026/7/13.
//

export module yuri.ui.widgets:context_menu;

import :base;
import yuri.core;
import yuri.skia;
import yuri.ui.render;
import yuri.ui.layout.core;
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
  std::string name_; // 菜单项名称
};

/** 通用弹出式上下文菜单 */
class ContextMenu final : public Widget {
public:
  /** 创建上下文菜单 */
  explicit ContextMenu(std::vector<ContextMenuItem> items, Widget *anchor);

  /** 销毁上下文菜单 */
  ~ContextMenu() override;

  /** 更新菜单项 */
  void set_items(std::vector<ContextMenuItem> items);

  /** 在鼠标当前位置弹出菜单 */
  void popup_at_cursor();

  /** 在窗口指定位置弹出菜单 */
  void popup(float x, float y);

  /** 隐藏菜单 */
  void hide();

  /** 隐藏当前处于活动状态的上下文菜单 */
  static void hide_active();

  /** 绘制上下文菜单 */
  void paint(SkCanvas *canvas) override;

  /** 菜单项点击事件 */
  Signal<std::size_t> item_clicked{};

protected:
  /** 更新菜单项悬浮状态 */
  void on_mouse_move(float x, float y) override;

  /** 清除菜单项悬浮状态 */
  void on_mouse_leave(float x, float y) override;

  /** 处理菜单项点击 */
  void on_mouse_left_released(float x, float y) override;

private:
  /** 根据坐标获取菜单项索引 */
  [[nodiscard]] int item_index_at(float x, float y) const noexcept;

  /** 获取菜单当前高度 */
  [[nodiscard]] float menu_height() const noexcept;

  inline static ContextMenu *active_menu_ = nullptr; // 当前活动菜单
  std::vector<ContextMenuItem> items_;               // 菜单项
  std::vector<RenderText> item_texts_{};             // 菜单项渲染节点
  float menu_width_ = kDefaultMenuWidth;             // 菜单宽度
  int hovered_item_ = -1;                            // 当前悬浮菜单项
};

ContextMenuItem::ContextMenuItem(std::string name) : name_(std::move(name)) {
}

const std::string &ContextMenuItem::name() const noexcept {
  return name_;
}

ContextMenu::ContextMenu(std::vector<ContextMenuItem> items, Widget *anchor) :
  Widget(anchor == nullptr ? nullptr : anchor->root_widget()) {
  set_items(std::move(items));
  set_visible(false);
}

ContextMenu::~ContextMenu() {
  if (active_menu_ == this) {
    active_menu_ = nullptr;
  }
}

void ContextMenu::set_items(std::vector<ContextMenuItem> items) {
  items_ = std::move(items);
  item_texts_.clear();
  item_texts_.reserve(items_.size());

  for (std::size_t index = 0; index < items_.size(); ++index) {
    auto &text = item_texts_.emplace_back(items_[index].name());
    text.set_font_size(13.5f);
    text.set_color(kMenuTextColor);
    text.set_alignment(Alignment::CenterLeft);
    text.update(
      SkRect::MakeXYWH(
        kOuterPadding + 18.0f,
        kOuterPadding + kMenuPadding + static_cast<float>(index) * kMenuItemHeight,
        menu_width_ - 36.0f,
        kMenuItemHeight
      )
    );
  }

  set_geometry(
    x(),
    y(),
    menu_width_ + kOuterPadding * 2.0f,
    menu_height() + kOuterPadding * 2.0f
  );
  if (items_.empty()) {
    hide();
  }
}

void ContextMenu::popup_at_cursor() {
  const auto cursor_position = window()->cursorPosition();
  popup(cursor_position.x(), cursor_position.y());
}

void ContextMenu::popup(const float x, const float y) {
  if (items_.empty() || parent_ == nullptr) {
    return;
  }

  if (active_menu_ != nullptr && active_menu_ != this) {
    active_menu_->hide();
  }

  const float menu_x = std::clamp(x, 0.0f, std::max(0.0f, parent_->width() - menu_width_));
  float menu_y = y;
  if (menu_y + menu_height() > parent_->height()) {
    menu_y = y - menu_height() + 1.0f;
  }
  menu_y = std::clamp(menu_y, 0.0f, std::max(0.0f, parent_->height() - menu_height()));

  move(menu_x - kOuterPadding, menu_y - kOuterPadding);
  set_visible(true);
  hovered_ = true;
  active_menu_ = this;
}

void ContextMenu::hide() {
  set_visible(false);
  hovered_ = false;
  hovered_item_ = -1;
  window()->setCursor(CursorType::Arrow);
  if (active_menu_ == this) {
    active_menu_ = nullptr;
  }
}

void ContextMenu::hide_active() {
  if (active_menu_ != nullptr) {
    active_menu_->hide();
  }
}

void ContextMenu::paint(SkCanvas *canvas) {
  const auto menu_rect = SkRect::MakeXYWH(kOuterPadding, kOuterPadding, menu_width_, menu_height());

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
  if (hovered_item_ >= 0) {
    // ReSharper disable once CppDFAUnreachableCode
    SkPaint hover;
    hover.setAntiAlias(true);
    hover.setColor(kMenuHoverColor);
    const float hover_y =
      kOuterPadding + kMenuPadding + static_cast<float>(hovered_item_) * kMenuItemHeight;
    canvas->drawRoundRect(
      SkRect::MakeXYWH(
        kOuterPadding + kMenuPadding, hover_y, menu_width_ - kMenuPadding * 2.0f, kMenuItemHeight
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

  for (auto &text : item_texts_) {
    text.draw(canvas);
  }
}

void ContextMenu::on_mouse_move(const float x, const float y) {
  hovered_item_ = item_index_at(x, y);
  window()->setCursor(hovered_item_ >= 0 ? CursorType::Hand : CursorType::Arrow);
}

void ContextMenu::on_mouse_leave(float x, float y) {
  hide();
}

void ContextMenu::on_mouse_left_released(const float x, const float y) {
  const int clicked_item = item_index_at(x, y);
  hide();
  if (clicked_item >= 0) {
    item_clicked.emit(static_cast<std::size_t>(clicked_item));
  }
}

int ContextMenu::item_index_at(const float x, const float y) const noexcept {
  const float content_x = x - kOuterPadding;
  const float content_y = y - kOuterPadding;
  if (
    content_x < kMenuPadding || content_x >= menu_width_ - kMenuPadding || content_y < kMenuPadding
    || content_y >= menu_height() - kMenuPadding
  ) {
    return -1;
  }

  const auto index = static_cast<std::size_t>((content_y - kMenuPadding) / kMenuItemHeight);
  return index < items_.size() ? static_cast<int>(index) : -1;
}

float ContextMenu::menu_height() const noexcept {
  return kMenuPadding * 2.0f + static_cast<float>(items_.size()) * kMenuItemHeight;
}

} // namespace ui::widgets
