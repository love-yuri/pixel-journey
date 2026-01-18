export module ui.render:border;

import :base;
import skia.resource;
import std;

using namespace skia;

export namespace ui::render {

class RenderBorder : public RenderNode {
  using RenderNode::RenderNode;
  SkPaint paint = PaintDesc {
    .color = skia_colors::gray,
    .style = SkPaint::kStroke_Style
  };

public:
  float radius = 0; // 圆角大小

  /**
   * 设置border宽度
   * @param width 宽度
   */
  void set_border_width(float width);

  /**
   * render
   */
  void render(SkCanvas* canvas) override;
};

void RenderBorder::set_border_width(const float width) {
  paint.setStrokeWidth(width);
}

void RenderBorder::render(SkCanvas *canvas) {
  if (!visible) {
    return;
  }

  if (radius <= 0) {
    canvas->drawRect(local_rect_, paint);
  } else {
    canvas->drawRoundRect(local_rect_, radius, radius, paint);
  }
}

} // namespace ui::render
