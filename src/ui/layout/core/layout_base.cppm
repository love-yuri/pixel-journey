//
// Created by love-yuri on 2026/1/26.
//

export module ui.layout:base;

import std;

export namespace ui::layout {

template <typename Widget>
class Layout {
public:
  virtual ~Layout() = default;
  virtual void apply(Widget* widget) const = 0;
};

}