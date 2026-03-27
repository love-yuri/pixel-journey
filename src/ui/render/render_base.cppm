//
// Created by love-yuri on 2026/1/14.
//
module;
#if defined(_WIN32)
// ReSharper disable once CppUnusedIncludeDirective
#include "include/private/base/SkFloatingPoint.h"
#endif
export module ui.render:base;

import skia.api;
import skia.resource;
import ui.layout;
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
  Alignment alignment_ = Alignment::TopLeft;
  Insets padding_{};

  /**
   * 根据对齐方式计算内容位置
   * @param content  内容的本地边界 (如 {0,0,w,h} 或 font_rect)
   * @return 内容在 self_box 中的绘制起点
   */
  [[nodiscard]] SkPoint calcAlignedPosition(const SkRect &content) const {
    float x = 0, y = 0;

    // 水平
    if (alignment_ & Alignment::Left) {
      x = self_box.fLeft + padding_.left - content.fLeft;
    } else if (alignment_ & Alignment::Right) {
      x = self_box.fRight - padding_.right - content.fRight;
    } else if (alignment_ & Alignment::HCenter) {
      x = self_box.centerX() - content.centerX();
    }

    // 垂直
    if (alignment_ & Alignment::Top) {
      y = self_box.fTop + padding_.top - content.fTop;
    } else if (alignment_ & Alignment::Bottom) {
      y = self_box.fBottom - padding_.bottom - content.fBottom;
    } else if (alignment_ & Alignment::VCenter) {
      y = self_box.centerY() - content.centerY();
    }

    return {x, y};
  }

public:
  bool visible = true;

  RenderNode() noexcept = default;
  explicit RenderNode(const SkRect &rect) : self_box(rect) {
  }

  virtual ~RenderNode() = default;

  virtual void render(SkCanvas *canvas) = 0;

  virtual void update() {
  }

  void update(const SkRect &rect) {
    self_box = rect;
    update();
  }

  void setAlignment(const Alignment align) {
    alignment_ = align;
    update();
  }

  void setPadding(const Insets &insets) {
    padding_ = insets;
    update();
  }

  void setPadding(float left, float top, float right, float bottom) {
    padding_ = {left, top, right, bottom};
    update();
  }

  [[nodiscard]] float left() const {
    return self_box.fLeft;
  }

  [[nodiscard]] float right() const {
    return self_box.fRight;
  }
};

} // namespace ui::render
