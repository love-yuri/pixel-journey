//
// Created by love-yuri on 2026/1/14.
//

export module ui.render:base;

import skia.api;
import skia.resource;
import ui.core;

using namespace skia;

export namespace ui::render {

/**
 * 初始化paint 默认抗锯齿
 * @return 初始化后的paint
 */
struct PaintDesc {
  SkColor color = skia_colors::black;
  float strokeWidth = 2.0f;
  bool antiAlias = true;
  SkPaint::Style style = SkPaint::kFill_Style;

  // 隐式转换为 SkPaint
  // ReSharper disable once CppNonExplicitConversionOperator
  operator SkPaint() const {
    SkPaint p;
    p.setColor(color);
    p.setStrokeWidth(strokeWidth);
    p.setAntiAlias(antiAlias);
    p.setStyle(style);
    return p;
  }
};

/**
 * render node基类
 */
class RenderNode: public core::RectTransform {
public:
  /**
  * 是否显示
  * 默认为true
  */
  bool visible = true;

  RenderNode() = default;
  explicit RenderNode(const SkRect& rect) {
    local_rect_ = rect;
  }

  /**
   * 绘制改node
   * @param canvas canvas
   */
  virtual void render(SkCanvas* canvas) = 0;

};

} // namespace ui::render