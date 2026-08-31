//
// Created by love-yuri on 2026/1/26.
//

export module yuri.ui.layout:base;

import std;
import :layout_dirty;
import yuri.skia.api;

export namespace ui::layout {

template <typename Widget>
class Layout {
protected:
  Widget *widget_; // 控件指针
  float spacing_ = 0; // 子控件间距

  /**
   * 直接设置控件几何状态 - 仅布局系统可用 (通过 friend)
   */
  static void setGeometry(Widget *widget, float x, float y, float width, float height) noexcept {
    widget->x_ = x;
    widget->y_ = y;
    widget->width_ = width;
    widget->height_ = height;
    widget->markLayoutDirty();
  }

public:
  explicit Layout(Widget *widget) {
    if (widget == nullptr) {
      throw std::invalid_argument("widget is null!");
    }
    widget_ = widget;
    widget->markLayoutDirty();
  }

  virtual ~Layout() = default;
  virtual void apply() const = 0;

  // 设置子控件间距
  void setSpacing(const float spacing) noexcept {
    spacing_ = spacing;
  }

  // 获取子控件间距
  float spacing() const noexcept {
    return spacing_;
  }
};

} // namespace ui::layout
