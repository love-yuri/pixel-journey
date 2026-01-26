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
import signal;
import ui.animation;

using namespace ui::render;
using namespace ui::layout;
using namespace skia;
using namespace ui::animation;

export namespace ui::widgets {

class Button : public Box {
  RenderText render_text;     // 字体节点
  bool is_clicked = false;    // 是否被点击
  SkPoint last_point{};       // 上次被处理的点

public:
  Signal<> clicked; // 点击事件

  explicit Button(std::string_view text, Widget *parent);

  void onClick() {
  }

  /**
   * 绘制
   */
  void paint(SkCanvas *canvas) override;

  /**
   * 返回字体节点
   */
  inline RenderText& text() noexcept {
    return render_text;
  }

  void onMouseMove(const float x, const float y) override {
    if (!is_clicked) {
      return;
    }

    move(self_box.x() + x - last_point.x(), self_box.y() + y - last_point.y());
    render_text.reCalculate();
  }

  void onMouseEnter(float x, float y) override {
    animation_manager->start(0.f, 12.f, 200,  &radius);
  }

  void onMouseLeave(float x, float y) override {
    animation_manager->start(12.f, 0.f, 200,  &radius);
  }

  void onMouseLeftPressed(const float x, const float y) override {
    is_clicked = true;
    is_dragging = true;
    last_point.set(x, y);

    yuri::info("move x: {}， y: {}", x, y);
  }

  void onMouseLeftReleased(float x, float y) override {
    is_clicked = false;
    is_dragging = false;
    clicked.emit();

    yuri::info("move x: {}， y: {}", x, y);
  }

  void layoutChildren() override;
};

Button::Button(const std::string_view text, Widget *parent) : Box(parent), render_text(&self_box) {
  render_text.setTextAndAlignment(text, Alignment::Center);
  render_bg.setColor(skia_colors::light_pink);

  // 连接信号
  clicked.connect<&Button::onClick>(this);
}

void Button::paint(SkCanvas *canvas) {
  render_bg.render(canvas);
  render_border.render(canvas);
  render_text.render(canvas);
}

void Button::layoutChildren() {
  Box::layoutChildren();
  render_text.reCalculate();
}

} // namespace ui::widgets