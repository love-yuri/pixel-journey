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
import yuri.ui.animation;

using namespace ui::render;
using namespace ui::layout;
using namespace skia;
using namespace ui::animation;

export namespace ui::widgets {

class Button : public Box {
  RenderText render_text_;  // 字体节点
  bool is_clicked_ = false; // 是否被点击
  SkPoint last_point_{};    // 上次被处理的点

public:
  Signal<> clicked; // 点击事件

  explicit Button(std::string_view text, Widget *parent);

  /**
   * 绘制
   */
  void paint(SkCanvas *canvas) override;

  void layout_children() override;

  /**
   * 返回字体节点
   */
  RenderText& text() noexcept {
    return render_text_;
  }

protected:
  // NOLINTNEXTLINE(readability-make-member-function-const)
  void on_mouse_move(const float x, const float y) override {
    if (!is_clicked_) {
      return;
    }

    move(x_ + x - last_point_.x(), y_ + y - last_point_.y());
  }

  void on_mouse_enter(float x, float y) override {
    // animation_manager->start(0.f, 12.f, 200,  &radius);
  }

  void on_mouse_leave(float x, float y) override {
    // animation_manager->start(12.f, 0.f, 200,  &radius);
  }

  void on_mouse_left_pressed(const float x, const float y) override {
    is_clicked_ = true;
    is_dragging_ = true;
    last_point_.set(x, y);
  }

  void on_mouse_left_released(float x, float y) override {
    is_clicked_ = false;
    is_dragging_ = false;
    clicked.emit();
  }
};

Button::Button(const std::string_view text, Widget *parent) : Box(parent) {
  render_text_.set_text_and_alignment(text, Alignment::Center);
  render_bg.set_color(ColorFromARGB(0xff, 145, 209, 123));

  radius = 4;

  // 默认大小要能够显示字体
  const auto &text_bound = render_text_.text_bound();
  resize(text_bound.width() + 32, text_bound.height() + 18);
}

void Button::paint(SkCanvas *canvas) {
  render_bg.draw(canvas);
  render_border.draw(canvas);
  render_text_.draw(canvas);
}

void Button::layout_children() {
  render_text_.update(local_bounds());
  render_bg.update(local_bounds());
  render_border.update(local_bounds());
}

} // namespace ui::widgets
