//
// Created by love-yuri on 2026/1/15.
//

module;
// MSVC 模块中使用 SkRect 的中心计算时，需要此静态内联函数定义。
#include "include/private/SkFloatingPoint.h"
export module yuri.ui.layout:alignment;

import yuri.skia.api;

using namespace skia;

export namespace ui::layout {

/**
 * 水平对齐方式，单独选择左对齐、居中或右对齐，不使用位掩码。
 */
enum class HorizontalAlignment : unsigned char {
  Left,   // 左对齐
  Center, // 水平居中
  Right,  // 右对齐
};

/**
 * 垂直对齐方式，单独选择顶部对齐、居中或底部对齐，不使用位掩码。
 */
enum class VerticalAlignment : unsigned char {
  Top,    // 顶部对齐
  Center, // 垂直居中
  Bottom, // 底部对齐
};

/**
 * 水平与垂直对齐方式的组合，默认左上对齐。
 */
struct Alignment {
  HorizontalAlignment horizontal = HorizontalAlignment::Left; // 水平对齐
  VerticalAlignment vertical = VerticalAlignment::Top;        // 垂直对齐

  // 同类型静态常量在类体外完成 constexpr 定义。
  static const Alignment TopLeft;      // 左上对齐
  static const Alignment TopCenter;    // 顶部居中
  static const Alignment TopRight;     // 右上对齐
  static const Alignment CenterLeft;   // 垂直居中、水平靠左
  static const Alignment Center;       // 水平与垂直居中
  static const Alignment CenterRight;  // 垂直居中、水平靠右
  static const Alignment BottomLeft;   // 左下对齐
  static const Alignment BottomCenter; // 底部居中
  static const Alignment BottomRight;  // 右下对齐

  /**
   * 重载==
   */
  bool operator==(const Alignment &alignment) const {
    return horizontal == alignment.horizontal && vertical == alignment.vertical;
  }

  /**
   * 计算将内容边界对齐到目标区域所需的平移量，不缩放或裁剪内容。
   * 目标区域应由调用方预先扣除内边距；内容边界可具有非零原点，如字体边界。
   * @param area 目标区域，坐标与返回的平移量使用相同单位
   * @param content 内容的本地边界，应用返回的平移量后与目标区域对齐
   * @return 应用于内容本地坐标的平移量，不一定等于平移后内容矩形的左上角
   */
  [[nodiscard]] SkPoint calc_aligned_position(SkRect area, SkRect content) const noexcept;
};

constexpr Alignment Alignment::TopLeft = { // 左上对齐
  .horizontal = HorizontalAlignment::Left,
  .vertical = VerticalAlignment::Top,
};

constexpr Alignment Alignment::TopCenter = { // 顶部居中
  .horizontal = HorizontalAlignment::Center,
  .vertical = VerticalAlignment::Top,
};

constexpr Alignment Alignment::TopRight = { // 右上对齐
  .horizontal = HorizontalAlignment::Right,
  .vertical = VerticalAlignment::Top,
};

constexpr Alignment Alignment::CenterLeft = { // 垂直居中、水平靠左
  .horizontal = HorizontalAlignment::Left,
  .vertical = VerticalAlignment::Center,
};

constexpr Alignment Alignment::Center = { // 水平与垂直居中
  .horizontal = HorizontalAlignment::Center,
  .vertical = VerticalAlignment::Center,
};

constexpr Alignment Alignment::CenterRight = { // 垂直居中、水平靠右
  .horizontal = HorizontalAlignment::Right,
  .vertical = VerticalAlignment::Center,
};

constexpr Alignment Alignment::BottomLeft = { // 左下对齐
  .horizontal = HorizontalAlignment::Left,
  .vertical = VerticalAlignment::Bottom,
};

constexpr Alignment Alignment::BottomCenter = { // 底部居中
  .horizontal = HorizontalAlignment::Center,
  .vertical = VerticalAlignment::Bottom,
};

constexpr Alignment Alignment::BottomRight = { // 右下对齐
  .horizontal = HorizontalAlignment::Right,
  .vertical = VerticalAlignment::Bottom,
};

SkPoint Alignment::calc_aligned_position(const SkRect area, const SkRect content) const noexcept {
  float x = 0.f, y = 0.f;

  // 对齐对应边缘或中心，同时抵消内容边界自身的坐标偏移。
  switch (horizontal) {
    case HorizontalAlignment::Left:
      x = area.fLeft - content.fLeft;
      break;
    case HorizontalAlignment::Center:
      x = area.centerX() - content.centerX();
      break;
    case HorizontalAlignment::Right:
      x = area.fRight - content.fRight;
      break;
  }

  switch (vertical) {
    case VerticalAlignment::Top:
      y = area.fTop - content.fTop;
      break;
    case VerticalAlignment::Center:
      y = area.centerY() - content.centerY();
      break;
    case VerticalAlignment::Bottom:
      y = area.fBottom - content.fBottom;
      break;
  }

  return {.fX = x, .fY = y};
}

} // namespace ui::layout
