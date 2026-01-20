//
// Created by yuri on 2026/1/19.
//
export module ui.animation:core;

import std;
import yuri_log;

using namespace std::chrono;
using namespace std::chrono_literals;
using duration = steady_clock::duration;

export namespace ui::animation {

template <typename T>
concept CanAnimation = requires(T a, T b, float t) { a + b; a - b; a * t; };

template <CanAnimation T>
class IAnimation {
protected:
  std::vector<T> from{};                   // 初始值容器
  std::vector<T> to{};                     // 目标值容器
  std::vector<T *> values{};               // value容器
  std::vector<float> inv_duration{};       // 持续时间 inv容器 1/ 500 ...
  std::vector<std::uint64_t> start_time{}; // 开始时间容器
  std::mutex mutex{};                      // 锁-单线程暂未考虑

  /**
  * 交换移除元素
  * @param i pos
  */
  void swapRemove(size_t i);

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
};

template <CanAnimation T>
void IAnimation<T>::start(const std::uint64_t now, const T& from_val, const T& to_val, const float dur, T *val_ptr) {
  from.emplace_back(from_val);
  to.emplace_back(to_val);
  inv_duration.emplace_back(1.f / dur / 1000.f);
  start_time.emplace_back(now);
  values.emplace_back(val_ptr);
}

template <CanAnimation T>
void IAnimation<T>::swapRemove(size_t i) {
  values[i] = values.back();
  values.pop_back();
  from[i] = from.back();
  from.pop_back();
  to[i] = to.back();
  to.pop_back();
  inv_duration[i] = inv_duration.back();
  inv_duration.pop_back();
  start_time[i] = start_time.back();
  start_time.pop_back();
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
