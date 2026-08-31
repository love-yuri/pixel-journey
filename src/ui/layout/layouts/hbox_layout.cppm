//
// Created by love-yuri on 2026/3/30.
//

export module yuri.ui.layout:hbox_layout;

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
    const auto total_spacing = this->spacing_ * static_cast<float>(children.size() - 1);
    float available = this->widget_->contentWidth() - total_spacing;
    auto remaining = static_cast<float>(children.size());

    float x = 0;
    for (std::size_t i = 0; i < children.size(); ++i) {
      const auto& child = children[i];
      const auto& sc = child->sizeConstraints();
      const float single_w = available / remaining;
      const auto w = sc.clampW(single_w);
      const auto h = sc.clampH(height);
      Layout<Widget>::setGeometry(child, x, 0, w, h);
      x += w + (i < children.size() - 1 ? this->spacing_ : 0);
      available -= w;
      remaining -= 1.0f;
    }
  }
};

}
