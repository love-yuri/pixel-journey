//
// Created by yuri on 2026/1/14.
//
module;

export module ui.widgets:button;

import std;
import ui.layout;
import ui.render;
import :base;
import :box;
import ui.animation;

using namespace ui::render;
using namespace ui::layout;
using namespace skia;

using namespace ui::animation;

export namespace ui::widgets {

class Button : public Box {
  RenderText render_text_;
  LinearAnimation<float> *animation_;

public:
  explicit Button(std::string_view text, Widget *parent);

  /**
   * 绘制
   */
  void paint(SkCanvas *canvas) override;

  void onMouseMove(float x, float y) override {
    // yuri::info("x: {}, y: {}", x, y);
  }

  void onMouseEnter(float x, float y) override {
    animation_manager->start(0.f, 30.f, 200,  &radius);
  }

  void onMouseLeave(float x, float y) override {
    animation_manager->start(30.f, 0.f, 200,  &radius);
  }

  void onMouseLeftPressed(float x, float y) override {
    animation_manager->start(0.f, 30.f, 100, this, &memberThunk<Box, float, &Box::setPadding>);
  }

  void onMouseLeftReleased(float x, float y) override {
    animation_manager->start(30.f, 0.f, 100, this, &memberThunk<Box, float, &Box::setPadding>);
  }

};

Button::Button(const std::string_view text, Widget *parent) : Box(parent) {
  constexpr auto rect_ = SkRect::MakeXYWH(250, 250, 200, 200);
  Box::setGeometry(rect_);
  render_text_.resize(width_, height_);
  render_text_.setText(text);
  render_text_.setAlignment(Alignment::Center);
}

void Button::paint(SkCanvas *canvas) {
  render_text_.render(canvas);
  render_border_.render(canvas);
}

} // namespace ui::widgets