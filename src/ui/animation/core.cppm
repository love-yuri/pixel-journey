//
// Created by yuri on 2026/1/19.
//
export module ui.animation:core;

import std;
import core;

using namespace std::chrono;
using namespace std::chrono_literals;
using duration = steady_clock::duration;

export namespace ui::animation {

/** 动画去重标识：(对象指针, setter函数指针) */
using AnimationTarget = std::pair<void *, void *>;

class IAnimation {
public:
  virtual ~IAnimation() = default;

  /**
   * 更新操作
   * @return 是否完成完整动画
   */
  virtual bool update(std::uint64_t now) = 0;

  /** 获取动画目标标识，用于去重 */
  [[nodiscard]] virtual AnimationTarget target() const noexcept = 0;
};

} // namespace ui::animation
