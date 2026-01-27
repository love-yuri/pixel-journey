//
// Created by yuri on 2026/1/27.
//

export module ui.layout:size_constraints;

import std;

export namespace ui::layout {

/**
 * 尺寸约束
 */
struct SizeConstraints {
  float min_w = 0.f;                                    // 最小宽度
  float min_h = 0.f;                                    // 最小高度
  float max_w = std::numeric_limits<float>::infinity(); // 最大宽度
  float max_h = std::numeric_limits<float>::infinity(); // 最大高度
};

} // namespace ui::layout