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
  SkRect content_box{};       // 内容box
  SkRect border_box{};        // border box
  Insets padding_{};          // 内边距

public:
  float &radius = render_border.radius; // 圆角

  /**
   * 默认构造函数
   */
  explicit Box(Widget *parent = nullptr);

  /**
   * 设置内边距
   */
  inline void setPadding(float padding) noexcept;

  /**
   * 设置内边距
   */
  inline void setPadding(const Insets& insets) noexcept;

protected:
  void layoutChildren() override;
};

inline void Box::setPadding(const float padding) noexcept {
  padding_.bottom = padding;
  markLayoutDirty(LayoutDirty::Self);
}

void Box::setPadding(const Insets& insets) noexcept {
  padding_ = insets;
  markLayoutDirty(LayoutDirty::Self);
}

void Box::layoutChildren() {
  content_box.setXYWH(padding_.left, padding_.top, width_ - padding_.left - padding_.right, height_ - padding_.top - padding_.bottom);
  border_box.fRight = width_;
  border_box.fBottom = height_;
}

Box::Box(Widget *parent): Widget(parent), render_border(&border_box) {
}

};
