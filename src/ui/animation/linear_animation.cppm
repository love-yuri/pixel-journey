//
// Created by yuri on 2026/1/19.
//

export module ui.animation:linear_animation;

import :core;
import yuri_log;
import profiling;
import std;

using namespace profiling;

export namespace ui::animation {

// 线性插值
template <CanAnimation T>
[[nodiscard]] T lerp(const T &from, const T &to, float t) noexcept {
  return from + (to - from) * t;
}

template <CanAnimation T>
class LinearAnimation : public IAnimation {
public:
  using Setter = function_ref<void(const T&)>;

  LinearAnimation(const T& from, const T& to, float duration, Setter setter) noexcept;

  /**
   * 更新参数
   */
  bool update(std::uint64_t now) override;

private:
  T from;              // 起始值
  T to;                // 目标值
  float inv_dur;       // 间隔比例 1 / (duration * 1000.0)
  std::uint64_t start; // 起始时间
  Setter setter;       // setter
};

template <CanAnimation T>
LinearAnimation<T>::LinearAnimation(const T &from, const T &to, const float duration, Setter setter) noexcept :
  from(from), to(to), inv_dur(1.f / (duration * 1000.f)), start(frame_clock.now), setter(std::move(setter)) {
}

template <CanAnimation T>
bool LinearAnimation<T>::update(const std::uint64_t now) {
  if (float t = static_cast<float>(now - start) * inv_dur; t >= 1.f) {
    setter(to);
    return true;
  } else {
    setter(lerp(from, to, t));
    return false;
  }
}

} // namespace ui::animation