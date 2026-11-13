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
   * 根据可用宽度和尺寸约束水平排列子控件，并预留外边距、应用垂直对齐方式。
   * @param parent 需要排列子控件的父控件
   */
  void apply(Widget &parent) const override;
};

void HBoxLayout::apply(Widget &parent) const {
  const auto &children = parent.children();
  if (children.empty()) return;

  const auto height = parent.height();
  const auto total_spacing = spacing_ * static_cast<float>(children.size() - 1);
  float available = parent.width() - total_spacing;
  // 先预留全部外边距，剩余主轴空间再按原有规则分配给控件本身。
  for (const auto *const child : children) {
    const auto margin = child->margin();
    available -= margin.left + margin.right;
  }
  auto remaining = static_cast<float>(children.size());

  float x = 0.f;
  for (std::size_t i = 0; i < children.size(); ++i) {
    auto *const child = children[i];
    const auto &constraints = child->size_constraints();
    const auto margin = child->margin();
    const auto width = constraints.clampW(available / remaining);
    const auto child_height = constraints.clampH(height - margin.top - margin.bottom);
    const auto slot_width = width + margin.left + margin.right;
    const auto slot_height = child_height + margin.top + margin.bottom;
    // 将包含外边距的布局槽对齐，再由 arrange 扣除外边距。
    const auto y = calc_aligned_offset(child->vertical_alignment(), height, slot_height); // 垂直偏移
    arrange(*child, skia::SkRect::MakeXYWH(x, y, slot_width, slot_height));
    x += slot_width + (i < children.size() - 1 ? spacing_ : 0.f);
    available -= width;
    remaining -= 1.f;
  }
}

} // namespace ui::layout
