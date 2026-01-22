//
// Created by yuri on 2026/1/20.
//

export module ui.widgets:box;

import std;
import ui.layout;
import ui.render;
import :base;
import ui.animation;

using namespace ui::render;
using namespace ui::layout;
using namespace skia;

using namespace ui::animation;

export namespace ui::widgets {

class Box : public Widget {
  using Widget::Widget;

protected:

  /* nodes */
  RenderBorder render_border_;

  float padding_left = 0; // 左侧内边距
  float padding_right = 0;
  float padding_top = 0;
  float padding_bottom = 0;

  float &radius = render_border_.radius;

public:
  inline void resize(float width, float height) noexcept override;
  inline void setGeometry(const SkRect &rect) noexcept override;
  inline void setGeometry(float x, float y, float width, float height) noexcept override;

  /**
   * 设置内边距
   */
  inline void setPadding(float padding) noexcept;

private:
  /**
   * 重新计算border的size
   */
  inline void borderResize() noexcept;
};

inline void Box::resize(const float width, const float height) noexcept {
  Widget::resize(width, height);
  borderResize();
}

inline void Box::setGeometry(const SkRect &rect) noexcept {
  Widget::setGeometry(rect);
  borderResize();
}

inline void Box::setGeometry(const float x, const float y, const float width, const float height) noexcept {
  Widget::setGeometry(x, y, width, height);
  borderResize();
}

inline void Box::setPadding(const float padding) noexcept {
  padding_left = padding_right = padding_top = padding_bottom = padding;
  borderResize();
}

inline void Box::borderResize() noexcept {
  render_border_.setGeometry(padding_left, padding_top, width_ - padding_left - padding_right, height_ - padding_top - padding_bottom);
}

};
