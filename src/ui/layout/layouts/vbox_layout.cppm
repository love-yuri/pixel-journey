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
    const auto total_spacing = this->spacing_ * static_cast<float>(children.size() - 1);
    float available = this->widget_->contentHeight() - total_spacing;
    auto remaining = static_cast<float>(children.size());

    float y = 0;
    for (std::size_t i = 0; i < children.size(); ++i) {
      const auto& child = children[i];
      const auto& sc = child->sizeConstraints();
      const float single_h = available / remaining;
      const auto w = sc.clampW(width);
      const auto h = sc.clampH(single_h);
      Layout<Widget>::setGeometry(child, 0, y, w, h);
      y += h + (i < children.size() - 1 ? this->spacing_ : 0);
      available -= h;
      remaining -= 1.0f;
    }
  }
};

}
