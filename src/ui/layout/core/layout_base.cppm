//
// Created by love-yuri on 2026/1/26.
//

export module yuri.ui.layout.core:base;

import yuri.skia.api;
import std;

export namespace ui::layout {

/**
 * 布局策略基类，仅通过受控入口向控件提交布局结果。
 * @tparam T 参与布局的控件类型
 */
template <typename T>
class Layout {
protected:
  float spacing_ = 0.f; // 子控件间距

  /**
   * 从布局槽中扣除子控件外边距，再提交控件几何信息和脏标记。
   * @param child 接收布局结果的子控件
   * @param rect 相对于父控件内容区域的布局槽，包含子控件外边距
   */
  static void arrange(T &child, const skia::SkRect &rect) noexcept;

public:
  /** 析构布局策略。 */
  virtual ~Layout() = default;

  /**
   * 根据父控件的内容区域排列子控件。
   * @param parent 需要排列子控件的父控件
   */
  virtual void apply(T &parent) const = 0;

  /**
   * 设置子控件间距。
   * @param spacing 子控件之间的间距
   */
  void set_spacing(float spacing) noexcept;

  /** 获取子控件间距。 */
  [[nodiscard]] float spacing() const noexcept {
    return spacing_;
  }
};

template <typename T>
void Layout<T>::arrange(T &child, const skia::SkRect &rect) noexcept {
  const auto margin = child.margin();
  // 外边距只影响布局槽，绘制和事件始终使用扣除外边距后的控件边界。
  child.apply_layout_geometry(skia::SkRect::MakeXYWH(
    rect.x() + margin.left,
    rect.y() + margin.top,
    std::max(0.f, rect.width() - margin.left - margin.right),
    std::max(0.f, rect.height() - margin.top - margin.bottom)
  ));
}

template <typename T>
void Layout<T>::set_spacing(const float spacing) noexcept {
  spacing_ = spacing;
}

} // namespace ui::layout
