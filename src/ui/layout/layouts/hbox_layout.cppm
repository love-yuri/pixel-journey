//
// Created by love-yuri on 2026/3/30.
//

export module yuri.ui.layout:hbox_layout;

import yuri.ui.layout.core;
import yuri.ui.widgets;
import yuri.skia.api;
import std;

using ui::widgets::Widget;

export namespace ui::layout {

/** 水平排列子控件的布局策略。 */
class HBoxLayout : public Layout<Widget> {
public:
  /**
   * 根据可用宽度和尺寸约束水平排列子控件，并应用各子控件的垂直对齐方式。
   * @param parent 需要排列子控件的父控件
   */
  void apply(Widget &parent) const override;
};

void HBoxLayout::apply(Widget &parent) const {
  const auto &children = parent.children();
  if (children.empty()) return;

  const auto height = parent.contentHeight();
  const auto total_spacing = spacing_ * static_cast<float>(children.size() - 1);
  float available = parent.contentWidth() - total_spacing;
  auto remaining = static_cast<float>(children.size());

  float x = 0.f;
  for (std::size_t i = 0; i < children.size(); ++i) {
    auto *const child = children[i];
    const auto &constraints = child->sizeConstraints();
    const auto width = constraints.clampW(available / remaining);
    const auto child_height = constraints.clampH(height);
    // 以扣除内边距后的内容区域计算交叉轴偏移，不影响主轴分配。
    const auto y = calc_aligned_offset(child->vertical_alignment(), height, child_height); // 垂直偏移
    arrange(*child, skia::SkRect::MakeXYWH(x, y, width, child_height));
    x += width + (i < children.size() - 1 ? spacing_ : 0.f);
    available -= width;
    remaining -= 1.f;
  }
}

} // namespace ui::layout
