//
// Created by love-yuri on 2026/1/14.
//

export module ui.render:base;

import skia.api;

export namespace ui::render {

/**
 * render node基类
 */
struct RenderNode {
  virtual ~RenderNode() = default;

  /**
   * 该node所占空间
   */
  skia::SkRect rect;

  /**
   * 绘制改node
   * @param canvas canvas
   */
  virtual void render(skia::SkCanvas* canvas) = 0;
};

}