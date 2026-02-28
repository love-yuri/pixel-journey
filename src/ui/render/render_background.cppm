//
// Created by love-yuri on 2026/1/22.
//

export module ui.render:background;

import :base;
import skia.resource;
import yuri_log;
import std;

using namespace skia;

export namespace ui::render {

class RenderBackground : public RenderNode {
  using RenderNode::RenderNode;
  SkPaint paint = PaintDesc {
    .color = skia_colors::transparent,
    .style = SkPaint::kFill_Style
  };

public:
  float *radius = nullptr; // 圆角大小
  void render(SkCanvas *canvas) override;

  /**
   * 设置边框颜色
   */
  inline void setColor(const SkColor c) noexcept {
    paint.setColor(c);
  }

  // 设置透明度
  inline void setOpacity(const float opacity) noexcept {
    paint.setAlphaf(opacity);
  }

};

void RenderBackground::render(SkCanvas *canvas) {
  if (!visible) {
    return;
  }

  if (radius == nullptr || *radius <= 0) {
    canvas->drawRect(self_box, paint);
  } else {
    canvas->drawRoundRect(self_box, *radius, *radius, paint);
  }
}

} // namespace ui::render