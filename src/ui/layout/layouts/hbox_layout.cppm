//
// Created by love-yuri on 2026/3/30.
//

export module ui.layout:hbox_layout;

import :base;
import :size_constraints;
import std;

export namespace ui::layout {

template <typename Widget>
class HBoxLayout : public Layout<Widget> {
public:
  explicit HBoxLayout(Widget* widget): Layout<Widget>(widget) {}

  void apply() const override {
    const auto& children = this->widget_->children();
    if (children.empty()) return;

    const auto height = this->widget_->contentHeight();
    const float single_w = this->widget_->contentWidth() / static_cast<float>(children.size());

    float x = 0;
    for (const auto& child : children) {
      const auto& sc = child->sizeConstraints();
      const auto w = sc.clampW(single_w);
      const auto h = sc.clampH(height);
      Layout<Widget>::setGeometry(child, x, 0, w, h);
      x += w;
    }
  }
};

}
