//
// Created by love-yuri on 2026/1/15.
//

export module ui.layout:alignment;

export namespace ui::layout {

/**
 * @brief 布局对齐方式 (位掩码格式)
 * 使用 unsigned char 占用 1 字节，支持位运算组合。
 */
enum class Alignment : unsigned char {
  Left = 1 << 0,    // 0000 0001
  Right = 1 << 1,   // 0000 0010
  Top = 1 << 2,     // 0000 0100
  Bottom = 1 << 3,  // 0000 1000
  HCenter = 1 << 4, // 0001 0000
  VCenter = 1 << 5, // 0010 0000

  TopLeft = Top | Left,         // 左上
  TopRight = Top | Right,       // 右上
  BottomLeft = Bottom | Left,   // 左下
  BottomRight = Bottom | Right, // 右下

  CenterLeft = VCenter | Left,     // 垂直居中，靠左
  CenterRight = VCenter | Right,   // 垂直居中，靠右
  TopCenter = HCenter | Top,       // 水平居中，靠顶
  BottomCenter = HCenter | Bottom, // 水平居中，靠底
  Center = HCenter | VCenter,      // 水平居中, 垂直居中
};

constexpr Alignment operator|(Alignment lhs, Alignment rhs) {
  return static_cast<Alignment>(
    static_cast<unsigned char>(lhs) | static_cast<unsigned char>(rhs));
}

constexpr bool operator&(Alignment lhs, Alignment rhs) {
  return static_cast<unsigned char>(lhs) & static_cast<unsigned char>(rhs);
}

} // namespace ui::layout
