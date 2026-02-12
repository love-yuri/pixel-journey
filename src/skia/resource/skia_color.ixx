//
// Created by love-yuri on 2026/1/13.
//

export module skia.resource:color;

import skia.api;

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

} // namespace skia_colors
