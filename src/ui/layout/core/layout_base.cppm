//
// Created by love-yuri on 2026/1/26.
//

export module yuri.ui.layout.core:base;

import yuri.skia.api;

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
   * 提交布局结果，由控件更新几何信息和脏标记。
   * @param child 接收布局结果的子控件
   * @param rect 相对于父控件内容区域的几何信息
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
  child.apply_layout_geometry(rect);
}

template <typename T>
void Layout<T>::set_spacing(const float spacing) noexcept {
  spacing_ = spacing;
}

} // namespace ui::layout
