//
// Created by love-yuri on 2026/1/26.
//

export module ui.layout:vbox_layout;

import :base;
import :size_constraints;
import yuri_log;
import std;

// import ui.widgets;
// using namespace ui::widgets;

export namespace ui::layout {

template <typename Widget>
class VBoxLayout : public Layout<Widget> {

public:
  explicit VBoxLayout(Widget* widget): Layout<Widget>(widget) {

  }

  void apply() const override {
    const auto &children = this->widget_->children();
    const auto width = this->widget_->contentWidth();
    const float single_h = this->widget_->contentHeight() / children.size();

    int y = 0;
    for (auto* child : children) {
      const auto& sizeConstraints = child->sizeConstraints();
      const auto w = std::clamp(width, sizeConstraints.min_w, sizeConstraints.max_w);
      const auto h = std::clamp(single_h, sizeConstraints.min_h, sizeConstraints.max_h);
      Layout<Widget>::setGeometry(child, 0, y, w, h);
      y += h;
    }
  }
};


}