//
// Created by love-yuri on 2026/1/14.
//
module;
#if defined(_WIN32)
// ReSharper disable once CppUnusedIncludeDirective
#include "include/private/SkFloatingPoint.h"
#endif
export module yuri.ui.render:base;

import yuri.skia.api;
import yuri.skia.resource;
import yuri.ui.layout;
import std;

using namespace skia;
using namespace ui::layout;

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
protected:
  SkRect self_box{};

public:
  bool visible = true;

  RenderNode() noexcept = default;
  explicit RenderNode(const SkRect &rect) : self_box(rect) {
  }

  virtual ~RenderNode() = default;

  /**
   * 绘制node
   */
  virtual void draw(SkCanvas *canvas) = 0;

  /**
   * 更新node的rect
   * @param rect 新的rect
   */
  virtual void update(SkRect rect) = 0;
};

} // namespace ui::render
