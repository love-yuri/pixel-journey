//
// Created by love-yuri on 2026/1/26.
//

export module ui.layout:layout_dirty;

import std;

export namespace ui::layout {

enum class LayoutDirty : std::uint8_t {
  None = 0,            // 没有改变
  Self = 1 << 0,       // 自身尺寸/位置改变
  Children = 1 << 1    // 子节点改变
};

inline LayoutDirty operator|(LayoutDirty a, LayoutDirty b) {
  return static_cast<LayoutDirty>(
      static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b)
  );
}

inline LayoutDirty& operator|=(LayoutDirty &a, LayoutDirty b) {
  a = a | b;
  return a;
}

}