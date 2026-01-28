//
// Created by love-yuri on 2026/1/26.
//

export module ui.layout:base;

import std;

export namespace ui::layout {

template <typename Widget>
class Layout {
protected:
  Widget* widget_; // 控件指针
public:
  explicit Layout(Widget* widget)  {
    if (widget == nullptr) {
      throw std::invalid_argument("widget is null!");
    }
    widget_ = widget;
    widget->markLayoutDirty();
  }

  virtual ~Layout() = default;
  virtual void apply() const = 0;
};

} // namespace ui::layout