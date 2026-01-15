//
// Created by yuri on 2026/1/14.
//

export module ui.widgets:button;

import std;
import skia.api;
import ui.layout;
import ui.render;

using namespace ui::render;
using namespace ui::layout;
using namespace skia;

export namespace ui::widgets {

class Button {
  RenderText render_text_;
  RenderBorder render_border_;

public:
  SkRect rect = SkRect::MakeXYWH(150, 250, 200, 200);

  explicit Button(std::string_view text);

  /**
   * 绘制
   */
  void render(SkCanvas* canvas);
};

Button::Button(const std::string_view text): render_text_(text, SkRect::MakeXYWH(150, 250, 200, 200)), render_border_(SkRect::MakeXYWH(150, 250, 200, 200)) {
}

void Button::render(SkCanvas *canvas) {
  render_text_.render(canvas);
  render_border_.render(canvas);

  render_text_.set_alignment(Alignment::Center);
}

} // namespace ui::control