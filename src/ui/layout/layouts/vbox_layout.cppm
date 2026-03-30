//
// Created by love-yuri on 2026/1/26.
//

export module ui.layout:vbox_layout;

import :base;
import :size_constraints;
import std;

export namespace ui::layout {

template <typename Widget>
class VBoxLayout : public Layout<Widget> {
public:
  explicit VBoxLayout(Widget* widget): Layout<Widget>(widget) {}

  void apply() const override {
    const auto& children = this->widget_->children();
    if (children.empty()) return;

    const auto width = this->widget_->contentWidth();
    const float single_h = this->widget_->contentHeight() / static_cast<float>(children.size());

    float y = 0;
    for (const auto& child : children) {
      const auto& sc = child->sizeConstraints();
      const auto w = sc.clampW(width);
      const auto h = sc.clampH(single_h);
      Layout<Widget>::setGeometry(child, 0, y, w, h);
      y += h;
    }
  }
};

}
