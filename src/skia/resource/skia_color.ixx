//
// Created by love-yuri on 2026/1/13.
//
module;
#include "include/core/SkColor.h"
export module skia.resource:color;

import skia.api;
import std;

#define SkColorGetA(color)      (((color) >> 24) & 0xFF)

/**
 * 常用颜色
 */
export namespace skia::skia_colors {

// 基础颜色
constexpr SkColor red = ColorFromARGB(0xFF, 0xFF, 0x00, 0x00);
constexpr SkColor green = ColorFromARGB(0xFF, 0x00, 0xFF, 0x00);
constexpr SkColor blue = ColorFromARGB(0xFF, 0x00, 0x00, 0xFF);

// 黑白灰
constexpr SkColor black = ColorFromARGB(0xFF, 0x00, 0x00, 0x00);
constexpr SkColor white = ColorFromARGB(0xFF, 0xFF, 0xFF, 0xFF);
constexpr SkColor gray = ColorFromARGB(0xFF, 0x80, 0x80, 0x80);
constexpr SkColor light_gray = ColorFromARGB(0xFF, 0xD3, 0xD3, 0xD3);
constexpr SkColor dark_gray = ColorFromARGB(0xFF, 0x40, 0x40, 0x40);

// 常用 UI 色
constexpr SkColor yellow = ColorFromARGB(0xFF, 0xFF, 0xFF, 0x00);
constexpr SkColor cyan = ColorFromARGB(0xFF, 0x00, 0xFF, 0xFF);
constexpr SkColor magenta = ColorFromARGB(0xFF, 0xFF, 0x00, 0xFF);
constexpr SkColor orange = ColorFromARGB(0xFF, 0xFF, 0xA5, 0x00);

// 柔和粉色系
constexpr SkColor pink = ColorFromARGB(0xFF, 0xFF, 0xC0, 0xCB);          // 经典粉
constexpr SkColor light_pink = ColorFromARGB(0xFF, 0xFF, 0xB6, 0xC1);    // 浅粉
constexpr SkColor misty_rose = ColorFromARGB(0xFF, 0xFF, 0xE4, 0xE1);    // 雾玫瑰
constexpr SkColor peach = ColorFromARGB(0xFF, 0xFF, 0xDA, 0xB9);         // 桃粉
constexpr SkColor lavender_blush = ColorFromARGB(0xFF, 0xFF, 0xF0, 0xF5);// 淡紫粉
constexpr SkColor rose = ColorFromARGB(0xFF, 0xF4, 0xC2, 0xC2);          // 玫瑰粉
constexpr SkColor coral_pink = ColorFromARGB(0xFF, 0xFF, 0xB3, 0xBA);    // 珊瑚粉

// 状态 / 功能色
constexpr SkColor transparent = ColorFromARGB(0x00, 0x00, 0x00, 0x00);
constexpr SkColor shadow = ColorFromARGB(0x80, 0x00, 0x00, 0x00);
constexpr SkColor disabled = ColorFromARGB(0xFF, 0xA0, 0xA0, 0xA0);

// 强调色
constexpr SkColor accent_blue = ColorFromARGB(0xFF, 0x00, 0x78, 0xD7);
constexpr SkColor accent_green = ColorFromARGB(0xFF, 0x00, 0xC8, 0x53);
constexpr SkColor accent_red = ColorFromARGB(0xFF, 0xE8, 0x11, 0x23);

SkColor LerpHSV(const SkColor c1, const SkColor c2, float t) {
  t = std::clamp(t, 0.0f, 1.0f);

  // 1. 转 HSV
  SkScalar hsv1[3], hsv2[3];
  SkColorToHSV(c1, hsv1);
  SkColorToHSV(c2, hsv2);

  // 2. 处理 Hue
  const float h1 = hsv1[0];
  const float h2 = hsv2[0];

  float dh = h2 - h1;
  if (dh > 180.0f) dh -= 360.0f;
  if (dh < -180.0f) dh += 360.0f;

  float h = h1 + dh * t;

  // 保证在 [0, 360)
  if (h < 0) h += 360.0f;
  if (h >= 360) h -= 360.0f;

  // 3. 插值 S / V
  const float s = hsv1[1] + (hsv2[1] - hsv1[1]) * t;
  const float v = hsv1[2] + (hsv2[2] - hsv1[2]) * t;

  // 4. 插值透明度
  const float a = SkColorGetA(c1) + static_cast<float>(SkColorGetA(c2) - SkColorGetA(c1)) * t;

  // 5. 转回 RGB
  const SkScalar hsv[3] = {h, s, v};
  return SkHSVToColor(static_cast<std::uint8_t>(a), hsv);
}

} // namespace skia_colors
