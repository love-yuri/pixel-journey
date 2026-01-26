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

protected:
  RenderBorder render_border; // border节点
  RenderBackground render_bg; // background节点

public:
  float radius = 0; // 圆角

  /**
   * 默认构造函数
   */
  explicit Box(Widget *parent = nullptr);

  /**
   * 返回border对象
   * @return border
   */
  inline RenderBorder& border() noexcept {
    return render_border;
  }


protected:
  void layoutChildren() override;
};

void Box::layoutChildren() {
  content_box.setXYWH(padding_.left, padding_.top, width_ - padding_.left - padding_.right, height_ - padding_.top - padding_.bottom);
}

Box::Box(Widget *parent): Widget(parent), render_border(&self_box), render_bg(&self_box) {
  render_border.radius = &radius;
  render_bg.radius = &radius;
}

};
