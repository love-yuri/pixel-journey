export module yuri.ui.render:border;

import :base;
import yuri.skia.resource;
import yuri.log;
import std;

using namespace skia;

export namespace ui::render {

class RenderBorder : public RenderNode {
  using RenderNode::RenderNode;
  SkPaint paint = PaintDesc {
    .color = skia_colors::gray,
    .style = SkPaint::kStroke_Style,
  };

public:
  float* radius = nullptr; // 圆角大小

  /**
   * render
   */
  void draw(SkCanvas* canvas) override;

  /**
   * 更新border边界
   */
  void update(SkRect rect) override;

  /**
   * 设置border宽度
   * @param width 宽度
   */
  void setWidth(const float width) noexcept {
    paint.setStrokeWidth(width);
  }

  /**
   * 设置边框颜色
   */
  void setColor(const SkColor c) noexcept {
    paint.setColor(c);
  }
};

void RenderBorder::draw(SkCanvas *canvas) {
  if (!visible) {
    return;
  }

  if (radius == nullptr || *radius <= 0) {
    canvas->drawRect(self_box, paint);
  } else {
    canvas->drawRoundRect(self_box, *radius, *radius, paint);
  }
}

void RenderBorder::update(const SkRect rect) {
  const auto insert = paint.getStrokeWidth() / 2;
  self_box.fLeft = rect.fLeft + insert;
  self_box.fTop = rect.fTop + insert;
  self_box.fRight = rect.fRight - insert;
  self_box.fBottom = rect.fBottom - insert;
}

} // namespace ui::render
