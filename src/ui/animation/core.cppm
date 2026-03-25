//
// Created by yuri on 2026/1/19.
//
export module ui.animation:core;

import std;
import signal;
import yuri_log;

using namespace std::chrono;
using namespace std::chrono_literals;
using duration = steady_clock::duration;

export namespace ui::animation {

class IAnimation {
public:
  virtual ~IAnimation() = default;

  /**
   * 更新操作
   * @return 是否完成完整动画
   */
  virtual bool update(std::uint64_t now) = 0;
};

template <typename T>
concept CanAnimation = requires(T a, T b, float t) { a + b; a - b; a * t; };

} // namespace ui::animation
