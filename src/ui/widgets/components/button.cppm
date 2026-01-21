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

public:
  explicit Button(std::string_view text, Widget *parent);

  /**
   * 绘制
   */
  void paint(SkCanvas *canvas) override;
  void resize(float width, float height) noexcept override;
  void setGeometry(const SkRect &rect) noexcept override;

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
  render_text_.resize(width_, height_);
  render_text_.setText(text);
  render_text_.setAlignment(Alignment::Center);
}

void Button::paint(SkCanvas *canvas) {
  render_text_.render(canvas);
  render_border_.render(canvas);
}

void Button::resize(const float width, const float height) noexcept {
  Box::resize(width, height);
  render_text_.resize(width, height);
}

void Button::setGeometry(const SkRect &rect) noexcept {
  Box::setGeometry(rect);
}

} // namespace ui::widgets