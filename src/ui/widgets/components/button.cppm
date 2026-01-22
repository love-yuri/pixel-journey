//
// Created by yuri on 2026/1/14.
//
module;

export module ui.widgets:button;

import std;
import ui.layout;
import ui.render;
import skia.resource;
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

  /**
   * 返回字体节点
   */
  inline RenderText text() noexcept {
    return render_text_;
  }

  void onMouseMove(float x, float y) override {
    move(x, y);
  }

  void onMouseEnter(float x, float y) override {
    animation_manager->start(0.f, 12.f, 200,  &radius);
  }

  void onMouseLeave(float x, float y) override {
    animation_manager->start(12.f, 0.f, 200,  &radius);
  }

  void onMouseLeftPressed(float x, float y) override {
    setPadding({0, 10, 0, 0});
  }

  void onMouseLeftReleased(float x, float y) override {
    setPadding(0);
  }

  void layoutChildren() override;
};

Button::Button(const std::string_view text, Widget *parent) : Box(parent), render_text_(&content_box) {
  render_text_.setTextAndAlignment(text, Alignment::Center);
  render_bg.setColor(skia_colors::green);
}

void Button::paint(SkCanvas *canvas) {
  render_bg.render(canvas);
  render_text_.render(canvas);
  render_border.render(canvas);

  yuri::info("x: {}, y: {}", self_box.x(), self_box.y());
}

void Button::layoutChildren() {
  Box::layoutChildren();
  render_text_.reCalculate();
}

} // namespace ui::widgets