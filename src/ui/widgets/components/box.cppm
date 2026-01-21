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
  void resize(float width, float height) noexcept override;
  void setGeometry(const SkRect &rect) noexcept override;

  /**
   * 设置内边距
   */
  void setPadding(float padding) noexcept;

private:
  /**
   * 重新计算border的size
   */
  void borderResize() noexcept;
};

void Box::resize(const float width, const float height) noexcept {
  Widget::resize(width, height);
  borderResize();
}

void Box::setGeometry(const SkRect &rect) noexcept {
  Widget::setGeometry(rect);
  borderResize();
}

void Box::setPadding(const float padding) noexcept {
  padding_left = padding_right = padding_top = padding_bottom = padding;
  borderResize();
}

void Box::borderResize() noexcept {
  render_border_.setGeometry(padding_left, padding_top, width_ - padding_left - padding_right, height_ - padding_top - padding_bottom);
}

};
