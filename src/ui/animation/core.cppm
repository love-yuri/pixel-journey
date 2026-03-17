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

template <typename T>
concept CanAnimation = requires(T a, T b, float t) { a + b; a - b; a * t; };

template <CanAnimation T>
struct Tween {
  using Setter = function_ref<void(const T&)>;

  float from;          // 起始值
  float to;            // 目标值
  float inv_dur;       // 间隔比例 1 / duration * 1000.0
  std::uint64_t start; // 起始时间
  Setter setter;       // setter
};

template <CanAnimation T>
class IAnimation {
public:
  using Setter = function_ref<void(const T&)>;

protected:
  std::vector<Tween<T>> values_{};      // setter

  /**
   * 交换移除元素
   * @param i pos
   */
  void swapRemove(std::size_t i);

public:

  virtual ~IAnimation() = default;

  /**
   * 更新操作
   */
  virtual void update(std::uint64_t now) = 0;

  /**
   * 开始一个新动画
   * @param now 当前时间
   * @param from_val 起始值
   * @param to_val 目标值
   * @param dur 持续时间
   * @param val_ptr 值value
   */
  void start(std::uint64_t now, const T& from_val, const T& to_val, float dur, T *val_ptr);

  /**
  * 开始一个新动画
  * @param now 当前时间
  * @param from_val 起始值
  * @param to_val 目标值
  * @param dur 持续时间
  * @param setter 回调函数
  */
  void start(std::uint64_t now, const T& from_val, const T& to_val, float dur, Setter setter);
};

template <CanAnimation T>
void IAnimation<T>::start(const std::uint64_t now, const T& from_val, const T& to_val, const float dur, T *val_ptr) {
  const auto func = [](void *p, const T &v) noexcept {
    *static_cast<T *>(p) = v;
  };

  start(now, from_val, to_val, dur, Setter { val_ptr, func });
}

template <CanAnimation T>
void IAnimation<T>::start(std::uint64_t now, const T &from_val, const T &to_val, const float dur, const Setter setter) {
  values_.emplace_back(from_val, to_val, 1.f / dur / 1000.f, now, std::move(setter));
}

template <CanAnimation T>
void IAnimation<T>::swapRemove(std::size_t i) {
  const auto swap_pop = [](auto &v, std::size_t idx) {
    v[idx] = std::move(v.back());
    v.pop_back();
  };
  swap_pop(values_, i);
}

struct FrameClock {
  /**
   * 当前帧时间 - 单位 微秒
   * 每帧render时更新，永远不要手动更新或者赋值
   * 提升访问性能，直接暴露接口
   */
  std::uint64_t now = 0;

  void update() {
    now = duration_cast<microseconds>(
      steady_clock::now().time_since_epoch()
    ).count();
  }
};

}
