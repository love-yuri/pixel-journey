//
// Created by love-yuri on 2026/1/26.
//

export module yuri.ui.layout:vbox_layout;

import yuri.ui.layout.core;
import yuri.ui.widgets;
import yuri.skia.api;
import std;

using ui::widgets::Widget;

export namespace ui::layout {

/** 垂直排列子控件的布局策略。 */
class VBoxLayout : public Layout<Widget> {
public:
  /**
   * 根据可用高度和尺寸约束垂直排列子控件，并预留外边距、应用水平对齐方式。
   * @param parent 需要排列子控件的父控件
   */
  void apply(Widget &parent) const override;
};

void VBoxLayout::apply(Widget &parent) const {
  const auto &children = parent.children();
  if (children.empty()) return;

  const auto width = parent.width();
  const auto total_spacing = spacing_ * static_cast<float>(children.size() - 1);
  float available = parent.height() - total_spacing;
  // 先预留全部外边距，剩余主轴空间再按原有规则分配给控件本身。
  for (const auto *const child : children) {
    const auto margin = child->margin();
    available -= margin.top + margin.bottom;
  }
  auto remaining = static_cast<float>(children.size());

  float y = 0.f;
  for (std::size_t i = 0; i < children.size(); ++i) {
    auto *const child = children[i];
    const auto &constraints = child->size_constraints();
    const auto margin = child->margin();
    const auto child_width = constraints.clampW(width - margin.left - margin.right);
    const auto height = constraints.clampH(available / remaining);
    const auto slot_width = child_width + margin.left + margin.right;
    const auto slot_height = height + margin.top + margin.bottom;
    // 将包含外边距的布局槽对齐，再由 arrange 扣除外边距。
    const auto x = calc_aligned_offset(child->horizontal_alignment(), width, slot_width); // 水平偏移
    arrange(*child, skia::SkRect::MakeXYWH(x, y, slot_width, slot_height));
    y += slot_height + (i < children.size() - 1 ? spacing_ : 0.f);
    available -= height;
    remaining -= 1.f;
  }
}

} // namespace ui::layout
