//
// Created by love-yuri on 2026/1/22.
//

export module ui.layout:insets;

export namespace ui::layout {

struct Insets {
  float left = 0, right = 0, top = 0, bottom = 0;
  constexpr Insets() = default;
  explicit constexpr Insets(const float all) : left(all), right(all), top(all), bottom(all) {
  }
  constexpr Insets(const float l, const float r, const float t, const float b) : left(l), right(r), top(t), bottom(b) {
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
    right = all;
  }
};

} // namespace ui::layout
