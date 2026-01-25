//
// Created by love-yuri on 2026/1/15.
//

export module ui.layout;

export import :alignment;
export import :insets;

export template <typename Widget>
struct VBoxLayout {

  void apply(Widget* widget) const {
    const auto children = widget->children();
    const auto width = widget->width();
    int y = 0;
    int h = widget->height() / children.size();

    for (auto* child : children) {
      child->setGeometry(0, y, width, h);
      y += h;
    }
  }
};
