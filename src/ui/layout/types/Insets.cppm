//
// Created by love-yuri on 2026/1/22.
//

export module ui.layout:insets;

export namespace ui::layout {

struct Insets {
  float left = 0, right = 0, top = 0, bottom = 0;
  constexpr Insets() = default;

  /**
   * 创建insets
   * @param all 所有
   */
  explicit constexpr Insets(const float all) : left(all), right(all), top(all), bottom(all) {
  }

  /**
   * 创建insets
   * @param l 左侧
   * @param t 顶部
   * @param r 右侧
   * @param b 底部
   */
  constexpr Insets(const float l, const float t, const float r, const float b) : left(l), right(r), top(t), bottom(b) {
  }

  /**
   * 创建insets
   * @param lr 左右两侧
   * @param tb 上下两侧
   */
  constexpr Insets(const float lr, const float tb) : left(lr), right(lr), top(tb), bottom(tb) {
  }

  inline Insets & operator=(const Insets &o) {
    left = o.left;
    right = o.right;
    top = o.top;
    bottom = o.bottom;
    return *this;
  }

  /**
   * 设置所有值
   */
  inline void setAll(const float all) noexcept {
    left = all;
    right = all;
    top = all;
    bottom = all;
  }
};

} // namespace ui::layout
