//
// Created by yuri on 2026/1/19.
//

export module ui.animation:linear_animation;

import :core;
import yuri_log;
import std;

export namespace ui::animation {

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
  for (std::size_t i = 0; i < this->values.size(); ++i) {
    if (const float t = (now - this->start_time[i]) * this->inv_duration[i]; t < 1.f) {
      const auto value = this->from[i] + (this->to[i] - this->from[i]) * t;
      this->values[i].apply(value);
    } else {
      this->values[i].apply(this->to[i]); // 更新为末尾值
      this->swapRemove(i);                // 弹出最后值
      --i;                                // 重置i
    }
  }
}


}