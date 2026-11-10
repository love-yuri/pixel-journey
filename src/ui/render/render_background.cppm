//
// Created by love-yuri on 2026/1/22.
//

export module yuri.ui.render:background;

import :base;
import yuri.skia.resource;

using namespace skia;

export namespace ui::render {

/** 绘制纯色矩形或圆角矩形的背景节点。 */
class RenderBackground final : public RenderNode {
  using RenderNode::RenderNode;

  SkPaint paint_ = PaintDesc{
    .color = skia_colors::transparent,
    .style = SkPaint::kFill_Style,
  }; // 背景绘制样式

public:
  float *radius = nullptr; // 圆角半径，空指针表示直角背景

  /**
   * 绘制背景，使用所属控件提供的本地坐标。
   * @param canvas 目标画布
   */
  void draw(SkCanvas *canvas) const override;

  /**
   * 更新背景的绘制区域。
   * @param rect 新的绘制区域
   */
  void update(SkRect rect) override;

  /**
   * 设置背景颜色。
   * @param color 背景颜色
   */
  void set_color(SkColor color) noexcept;

  /**
   * 设置背景透明度。
   * @param opacity 透明度，取值范围为 [0, 1]
   */
  void set_opacity(float opacity) noexcept;
};

void RenderBackground::draw(SkCanvas *canvas) const {
  if (!visible) {
    return;
  }

  if (radius == nullptr || *radius <= 0) {
    canvas->drawRect(self_box, paint_);
  } else {
    canvas->drawRoundRect(self_box, *radius, *radius, paint_);
  }
}

void RenderBackground::update(const SkRect rect) {
  self_box = rect;
}

void RenderBackground::set_color(const SkColor color) noexcept {
  paint_.setColor(color);
}

void RenderBackground::set_opacity(const float opacity) noexcept {
  paint_.setAlphaf(opacity);
}

} // namespace ui::render
