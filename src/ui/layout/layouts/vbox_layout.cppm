//
// Created by love-yuri on 2026/1/26.
//

export module ui.layout:vbox_layout;

import :base;
import :size_constraints;
import std;

// import ui.widgets;
// using namespace ui::widgets;

export namespace ui::layout {

template <typename Widget>
struct VBoxLayout : Layout<Widget> {
  void apply(Widget* widget) const override {
    const auto &children = widget->children();
    const auto width = widget->contentWidth();
    const float single_h = widget->contentHeight() / children.size();

    int y = 0;
    for (auto* child : children) {
      const auto& sizeConstraints = child->sizeConstraints();
      const auto w = std::clamp(width, sizeConstraints.min_w, sizeConstraints.max_w);
      const auto h = std::clamp(single_h, sizeConstraints.min_h, sizeConstraints.max_h);
      child->setGeometry(0, y, w, h);
      y += h;
    }
  }
};


}