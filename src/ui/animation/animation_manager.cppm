//
// Created by yuri on 2026/1/19.
//

export module ui.animation:animation_manager;

import :core;
import :linear_animation;
import std;

using namespace ui::animation;

// setter 函数
template <typename T>
void setter_fn(void *p, const T &v) noexcept {
  *static_cast<T *>(p) = v;
}

export namespace ui::animation {

/**
 * 动画管理
 * 使用方法: 1: animation_manager->start(0.f, 30.f, 100, this, &memberThunk<Box, float,
 * &Box::setPadding>);
 */
class AnimationManager {
public:
  /**
   * 更新渲染帧周期
   */
  void update();

  /**
   * 开启动画
   * @tparam T 参数类型
   * @param from 起始参数
   * @param to 目标参数
   * @param duration 持续时间-ms
   * @param value value指针
   */
  template <typename T>
  void start(const T &from, const T &to, float duration, T *value);

  /**
   * 开启动画
   * @param from 起始参数
   * @param to 目标参数
   * @param duration 持续时间-ms
   * @param obj this 对象
   */
  template <auto ptr, typename TObject, typename T>
  void start(const T &from, const T &to, float duration, TObject *obj);

private:
  std::vector<std::unique_ptr<IAnimation>> animations_; // 动画合集
};

template <typename T>
void AnimationManager::start(const T &from, const T &to, float duration, T *value) {
  // clang-format off
  animations_.emplace_back(
    std::make_unique<LinearAnimation<T>>(
      from, to, duration, typename LinearAnimation<T>::Setter{value, &setter_fn<T>}
    )
  );
  // clang-format on
}

template <auto ptr, typename TObject, typename T>
void AnimationManager::start(const T &from, const T &to, float duration, TObject *obj) {
  // clang-format off
  animations_.emplace_back(
    std::make_unique<LinearAnimation<T>>(
      from, to, duration, LinearAnimation<T>::Setter::template from<ptr>(obj)
    )
  );
  // clang-format off
}

void AnimationManager::update() {
  std::size_t i = 0;
  while (i < this->animations_.size()) {
    if (const auto &animation = animations_[i]; animation->update(frame_clock.now)) {
      animations_[i] = std::move(animations_.back());
      animations_.pop_back();
    } else {
      i++;
    }
  }
}

} // namespace ui::animation

auto animation_manager_ = std::make_shared<AnimationManager>();
export AnimationManager *animation_manager = animation_manager_.get();
