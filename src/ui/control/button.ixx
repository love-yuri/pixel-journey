//
// Created by yuri on 2026/1/14.
//

export module ui.control:button;

import std;
import skia.api;
import ui.render;

using namespace ui::render;
using namespace skia;

export namespace ui::control {

class Button {
  RenderBorder border_;
  RenderText render_text_;
  RenderBorder render_border_;

public:
  static constexpr auto rect = SkRect::MakeXYWH(150, 250, 200, 200);

  explicit Button(std::string_view text);

  /**
   * 绘制
   */
  void render(SkCanvas* canvas);
};

Button::Button(const std::string_view text) {

  render_text_.set_text(text);
  render_text_.rect = rect;
  render_border_.rect = rect;
  render_border_.radius = 12.0f;
}

void Button::render(SkCanvas *canvas) {
  render_text_.render(canvas);
  render_border_.render(canvas);
}

} // namespace ui::control