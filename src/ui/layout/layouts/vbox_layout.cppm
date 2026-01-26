//
// Created by love-yuri on 2026/1/26.
//

export module ui.layout:vbox_layout;

import :base;

export namespace ui::layout {

template <typename Widget>
struct VBoxLayout : Layout<Widget> {
  void apply(Widget* widget) const override {
    const auto &children = widget->children();
    const auto width = widget->width();
    int y = 0;
    int h = widget->height() / children.size();

    for (auto* child : children) {
      child->setGeometry(0, y, width, h);
      y += h;
    }
  }
};


}