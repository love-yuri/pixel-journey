//
// Created by yuri on 2026/1/19.
//

export module ui.animation:linear_animation;

import :core;
import yuri_log;
import std;

export namespace ui::animation {

// 线性插值
template <CanAnimation T>
[[nodiscard]] T lerp(const T &from, const T &to, float t) noexcept {
  return from + (to - from) * t;
}


template <CanAnimation T>
class LinearAnimation : public IAnimation<T> {
public:
  /**
   * 更新参数
   */
  void update(std::uint64_t now) override;
};

template <CanAnimation T>
void LinearAnimation<T>::update(const std::uint64_t now) {
  std::size_t i = 0;
  while (i < this->setters_.size()) {
    if (float t = static_cast<float>(now - this->start_[i]) * this->inv_dur[i]; t >= 1.f) {
      this->setters_[i](this->to_[i]);
      this->swapRemove(i);
    } else {
      this->setters_[i](lerp(this->from_[i], this->to_[i], t));
      ++i;
    }
  }
}


}