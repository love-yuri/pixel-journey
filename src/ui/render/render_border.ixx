export module ui.render:border;

import :base;
import skia.resource;
import std;

using namespace skia;

export namespace ui::render {

class RenderBorder : public RenderNode {
  SkPaint paint = [] {
    SkPaint paint;
    paint.setColor(skia_colors::gray);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(2);
    paint.setAntiAlias(true);
    return paint;
  }();

public:
  float radius = 0;

  void set_border(const float width) {
    paint.setStrokeWidth(width);
  }

  void render(SkCanvas* canvas) override {
    if (rect.isEmpty()) return;

    if (radius <= 0) {
      canvas->drawRect(rect, paint);
    } else {
      canvas->drawRoundRect(rect, radius, radius, paint);
    }
  }
};

} // namespace ui::render
