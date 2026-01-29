//
// Created by love-yuri on 2026/1/26.
//

export module ui.layout:base;

import std;
import :layout_dirty;
import skia.api;

export namespace ui::layout {

template <typename Widget>
class Layout {
protected:
  Widget* widget_; // 控件指针

  /**
   * 设置控件几何状态-内部或者给布局系统使用
   * @param widget 待设置的控件
   * @param rect 控件所占rect，基于父控件
   */
  static void setGeometry(Widget* widget, const skia::SkRect &rect) noexcept;

  /**
   * 设置控件几何状态-内部或者给布局系统使用
   * @param widget 待设置的控件
   * @param x 相对于父控件：x
   * @param y 相对于父控件: y
   * @param width 控件宽度
   * @param height 控件高度
   */
  static void setGeometry(Widget* widget, float x, float y, float width, float height) noexcept;

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

template <typename Widget>
void Layout<Widget>::setGeometry(Widget *widget, const skia::SkRect &rect) noexcept {
  widget->self_box = rect;
  widget->width_ = rect.width();
  widget->height_ = rect.height();
  widget->updateContentBox();
  widget->markLayoutDirty(LayoutDirty::Self);
}

template <typename Widget>
void Layout<Widget>::setGeometry(Widget *widget, const float x, const float y, const float width, float const height) noexcept {
  widget->width_ = width;
  widget->height_ = height;
  widget->self_box.setXYWH(x, y, width, height);
  widget->updateContentBox();
  widget->markLayoutDirty(LayoutDirty::Self);
}


} // namespace ui::layout