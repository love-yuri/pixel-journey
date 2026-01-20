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
  void resize(float width, float height) override;
  void setGeometry(const SkRect &rect) override;

  /**
   * 设置内边距
   */
  void setPadding(float padding);

private:
  /**
   * 重新计算border的size
   */
  void borderResize();
};

void Box::resize(const float width, const float height) {
  Widget::resize(width, height);
  borderResize();
}

void Box::setGeometry(const SkRect &rect) {
  Widget::setGeometry(rect);
  borderResize();
}

void Box::setPadding(const float padding) {
  padding_left = padding_right = padding_top = padding_bottom = padding;
  borderResize();
}

void Box::borderResize() {
  render_border_.setGeometry(padding_left, padding_top, width_ - padding_left - padding_right, height_ - padding_top - padding_bottom);
}

};
