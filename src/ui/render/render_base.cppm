//
// Created by love-yuri on 2026/1/14.
//

export module ui.render:base;

import skia.api;
import skia.resource;
import std;

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
class RenderNode {
public:
  /**
   * node 几何边界指针
   * 严格跟随调用widget
   */
  const SkRect *self_box{};

  /**
  * 是否显示
  * 默认为true
  */
  bool visible = true;

  RenderNode() noexcept = default;
  explicit RenderNode(const SkRect* rect): self_box(rect) {
    if (rect == nullptr) {
      throw std::runtime_error("node的边界不能为空!");
    }
  }

  virtual ~RenderNode() = default;

  /**
   * 绘制改node
   * @param canvas canvas
   */
  virtual void render(SkCanvas* canvas) = 0;
};

} // namespace ui::render