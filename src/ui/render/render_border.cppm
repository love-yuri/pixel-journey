export module ui.render:border;

import :base;
import skia.resource;
import yuri_log;
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
  float* radius = nullptr; // 圆角大小

  /**
   * render
   */
  void render(SkCanvas* canvas) override;

  /**
   * 设置border宽度
   * @param width 宽度
   */
  inline void setWidth(const float width) noexcept {
    paint.setStrokeWidth(width);
  }

  /**
   * 设置边框颜色
   */
  inline void setColor(const SkColor c) noexcept {
    paint.setColor(c);
  }
};

void RenderBorder::render(SkCanvas *canvas) {
  if (!visible) {
    return;
  }

  const auto insert = paint.getStrokeWidth() / 2;
  const SkRect rect = {
    self_box.fLeft + insert,
    self_box.fTop + insert,
    self_box.fRight - insert,
    self_box.fBottom - insert,
  };
  if (radius == nullptr || *radius <= 0) {
    canvas->drawRect(rect, paint);
  } else {
    canvas->drawRoundRect(rect, *radius, *radius, paint);
  }
}

} // namespace ui::render
