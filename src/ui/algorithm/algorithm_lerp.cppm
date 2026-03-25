//
// Created by love-yuri on 2026/3/25.
//

export module ui.algorithm:lerp;

import skia;

using namespace skia;

export namespace ui::algorithm {

template <typename T>
concept CanLerp = requires(T a, T b, float t) { a + b; a - b; a * t; };

template <CanLerp T>
[[nodiscard]] T lerp(const T& from, const T& to, float t) noexcept {
  return from + (to - from) * t;
}

template <>
[[nodiscard]] SkColor lerp<SkColor>(const SkColor& from, const SkColor& to, float t) noexcept {
  return skia_colors::LerpHSV(from, to, t);
}

}