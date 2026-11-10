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
   * 根据可用高度和尺寸约束垂直排列子控件。
   * @param parent 需要排列子控件的父控件
   */
  void apply(Widget &parent) const override;
};

void VBoxLayout::apply(Widget &parent) const {
  const auto &children = parent.children();
  if (children.empty()) return;

  const auto width = parent.contentWidth();
  const auto total_spacing = spacing_ * static_cast<float>(children.size() - 1);
  float available = parent.contentHeight() - total_spacing;
  auto remaining = static_cast<float>(children.size());

  float y = 0.f;
  for (std::size_t i = 0; i < children.size(); ++i) {
    auto *const child = children[i];
    const auto &constraints = child->sizeConstraints();
    const auto child_width = constraints.clampW(width);
    const auto height = constraints.clampH(available / remaining);
    arrange(*child, skia::SkRect::MakeXYWH(0.f, y, child_width, height));
    y += height + (i < children.size() - 1 ? spacing_ : 0.f);
    available -= height;
    remaining -= 1.f;
  }
}

} // namespace ui::layout
