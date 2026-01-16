//
// Created by yuri on 2026/1/14.
//

export module ui.widgets:button;

import std;
import skia.api;
import ui.layout;
import ui.render;
import :base;

using namespace ui::render;
using namespace ui::layout;
using namespace skia;

export namespace ui::widgets {

class Button : public Widget {
  SkRect rect;
  RenderText render_text_;
  RenderBorder render_border_;


public:
  explicit Button(std::string_view text, Widget *parent);

  /**
   * 绘制
   */
  void paint(SkCanvas *canvas) override;
};

Button::Button(const std::string_view text, Widget *parent) :
  Widget(SkRect::MakeXYWH(150, 250, 200, 200), parent),
  rect(SkRect::MakeXYWH(150, 250, 200, 200)),
  render_text_(text, rect),
  render_border_(rect) {
}

void Button::paint(SkCanvas *canvas) {
  render_text_.render(canvas);
  render_border_.render(canvas);

  render_text_.setAlignment(Alignment::Center);
}

} // namespace ui::widgets