//
// Created by yuri on 2026/1/19.
//

export module ui.animation:linear_animation;

import :clock;
import yuri_log;
import std;

export namespace ui::animation {

template <typename T>
struct LinearAnimation {
  T from;                        // 起点值
  T to;                          // 目标值
  float duration = 0;            // 持续时间，单位ms
  T *value = nullptr;            // 待修改的value
  const std::int64_t start_time; // 开始时间-仅初始化

  LinearAnimation(const std::int64_t now) : start_time(now) {
  }

  /**
   * 更新参数
   */
  bool update(const std::int64_t now) {
    const auto left = now - start_time;
    if (left >= duration * 1000) {
      return false;
    }

    *value = from + (to - from) * left / (duration * 1000.0);
    yuri::info("from: {}, to: {}, value: {}", from, to, *value);
    return true;
  }
};

}