//
// Created by yuri on 2026/1/14.
//
module;

export module yuri.ui.widgets:button;

import std;
import yuri.ui.layout.core;
import yuri.ui.render;
import yuri.skia.resource;
import :base;
import :box;
import yuri.core;

using namespace ui::render;
using namespace ui::layout;
using namespace skia;

namespace {

constexpr SkColor kBackgroundColor = 0xfffbfbfb;         // 默认背景色
constexpr SkColor kHoverBackgroundColor = 0xfff3f3f3;    // 悬停背景色
constexpr SkColor kPressedBackgroundColor = 0xffeaeaea;  // 按下背景色
constexpr SkColor kDisabledBackgroundColor = 0xfff5f5f5; // 禁用背景色
constexpr SkColor kBorderColor = 0xffd1d1d1;             // 默认边框色
constexpr SkColor kHoverBorderColor = 0xffb5b5b5;        // 悬停边框色
constexpr SkColor kPressedBorderColor = 0xffc4c4c4;      // 按下边框色
constexpr SkColor kDisabledBorderColor = 0xffe3e3e3;     // 禁用边框色
constexpr SkColor kTextColor = 0xff1a1a1a;               // 默认文字色
constexpr SkColor kPressedTextColor = 0xff606060;        // 按下文字色
constexpr SkColor kDisabledTextColor = 0xffa0a0a0;       // 禁用文字色
constexpr float kHorizontalPadding = 12.0f;              // 水平内边距
constexpr float kVerticalPadding = 6.0f;                 // 垂直内边距
constexpr float kBorderWidth = 1.0f;                     // 边框宽度
constexpr float kMinWidth = 80.0f;                       // 默认最小宽度
constexpr float kMinHeight = 32.0f;                      // 默认最小高度

} // namespace

export namespace ui::widgets {

/** 具有 Fluent 浅色默认样式和鼠标交互状态的按钮。 */
class Button : public Box {
public:
  Signal<> clicked; // 点击事件

  /**
   * 创建按钮，按文字大小设置初始尺寸。
   * @param text 按钮文字
   * @param parent 父控件
   */
  explicit Button(std::string_view text, Widget *parent = nullptr);

  /**
   * 绘制按钮背景、边框和文字。
   * @param canvas 目标画布
   */
  void paint(SkCanvas *canvas) override;

  /** 更新背景、边框和带内边距的文字区域。 */
  void layout_children() override;

  /** 获取按钮是否允许交互。 */
  [[nodiscard]] bool enabled() const noexcept {
    return enabled_;
  }

  /**
   * 设置按钮是否允许交互，禁用时取消当前按下状态。
   * @param enabled 是否允许交互
   */
  void set_enabled(bool enabled);

  /**
   * 返回文字节点，可用于修改文字和字号。
   */
  RenderText &text() noexcept {
    return render_text_;
  }

protected:
  /**
   * 鼠标进入时更新悬停样式。
   * @param x 鼠标本地横坐标
   * @param y 鼠标本地纵坐标
   */
  void on_mouse_enter(float x, float y) override;

  /**
   * 鼠标移出时恢复默认样式，保留本次按下记录以支持移回。
   * @param x 鼠标本地横坐标
   * @param y 鼠标本地纵坐标
   */
  void on_mouse_leave(float x, float y) override;

  /**
   * 记录按钮内的左键按下并显示按下样式。
   * @param x 鼠标本地横坐标
   * @param y 鼠标本地纵坐标
   */
  void on_mouse_left_pressed(float x, float y) override;

  /**
   * 结束按下状态，仅在按钮内松开有效的按下操作时触发点击。
   * @param x 鼠标本地横坐标
   * @param y 鼠标本地纵坐标
   */
  void on_mouse_left_released(float x, float y) override;

private:
  RenderText render_text_;  // 文字节点
  bool is_pressed_ = false; // 是否在按钮内按下了鼠标左键
  bool enabled_ = true;     // 是否允许交互

  /** 根据启用、悬停和按下状态更新默认样式。 */
  void update_visual_state();
};

Button::Button(const std::string_view text, Widget *parent) : Box(parent) {
  render_text_.set_font_size(14.0f);
  render_text_.set_text_and_alignment(text, Alignment::Center);
  render_border.setWidth(kBorderWidth);
  radius = 4.0f;
  update_visual_state();

  // 初始尺寸包含文字、内边距和边框，布局中也保留默认最小尺寸。
  const auto text_bound = render_text_.text_bound();
  set_min_size(kMinWidth, kMinHeight);
  resize(
    std::max(kMinWidth, text_bound.width() + 2.0f * (kHorizontalPadding + kBorderWidth)),
    std::max(kMinHeight, text_bound.height() + 2.0f * (kVerticalPadding + kBorderWidth))
  );
}

void Button::paint(SkCanvas *canvas) {
  render_bg.draw(canvas);
  render_border.draw(canvas);
  render_text_.draw(canvas);
}

void Button::layout_children() {
  render_text_.update(
    SkRect::MakeXYWH(
      kHorizontalPadding + kBorderWidth,
      kVerticalPadding + kBorderWidth,
      std::max(0.0f, width_ - 2.0f * (kHorizontalPadding + kBorderWidth)),
      std::max(0.0f, height_ - 2.0f * (kVerticalPadding + kBorderWidth))
    )
  );
  render_bg.update(local_bounds());
  render_border.update(local_bounds());
}

void Button::set_enabled(const bool enabled) {
  if (enabled_ == enabled) {
    return;
  }

  enabled_ = enabled;
  if (!enabled_) {
    is_pressed_ = false;
  }
  update_visual_state();
}

void Button::update_visual_state() {
  if (!enabled_) {
    render_bg.set_color(kDisabledBackgroundColor);
    render_border.setColor(kDisabledBorderColor);
    render_text_.set_color(kDisabledTextColor);
  } else if (is_pressed_ && hovered_) {
    render_bg.set_color(kPressedBackgroundColor);
    render_border.setColor(kPressedBorderColor);
    render_text_.set_color(kPressedTextColor);
  } else {
    render_bg.set_color(hovered_ ? kHoverBackgroundColor : kBackgroundColor);
    render_border.setColor(hovered_ ? kHoverBorderColor : kBorderColor);
    render_text_.set_color(kTextColor);
  }
}

void Button::on_mouse_enter(const float x, const float y) {
  update_visual_state();
}

void Button::on_mouse_leave(const float x, const float y) {
  update_visual_state();
}

void Button::on_mouse_left_pressed(const float x, const float y) {
  if (!enabled_) {
    return;
  }

  is_pressed_ = true;
  hovered_ = true;
  update_visual_state();
}

void Button::on_mouse_left_released(const float x, const float y) {
  hovered_ = local_bounds().contains(x, y);
  const bool should_click = enabled_ && is_pressed_ && hovered_;
  is_pressed_ = false;
  update_visual_state();

  if (should_click) {
    clicked.emit();
  }
}

} // namespace ui::widgets
