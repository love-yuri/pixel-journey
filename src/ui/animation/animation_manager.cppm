//
// Created by yuri on 2026/1/19.
//

export module ui.animation:animation_manager;

import :core;
import :linear_animation;
import std;

using namespace ui::animation;

export namespace ui::animation {

/**
 * 动画管理
 * 使用方法: 1: animation_manager->start(0.f, 30.f, 100, this, &memberThunk<Box, float, &Box::setPadding>);
 */
class AnimationManager {
  FrameClock clock; // 动画计时clock
  LinearAnimation<float> animations {};

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
  void start(float from, float to, float duration, float* value);

  /**
   * 开启动画
   * @tparam T 参数类型
   * @param from 起始参数
   * @param to 目标参数
   * @param duration 持续时间-ms
   * @param obj this 指针
   * @param func 回调函数指针
   */
  void start(float from, float to, float duration, void* obj, LinearAnimation<float>::memberFunc func);
};

void AnimationManager::start(const float from, const float to, const float duration, float *value) {
  animations.start(clock.now, from, to, duration, value);
}

void AnimationManager::start(const float from, const float to, const float duration, void* obj, LinearAnimation<float>::memberFunc func) {
  animations.start(clock.now, from, to, duration, obj, func);
}

} // namespace ui::animation

#ifdef __clang__
bool operator==(const std::vector<LinearAnimation<float> *>::iterator & it, const std::vector<LinearAnimation<float> *>::iterator & end) {
  return it.base() == end.base();
}
#endif

void AnimationManager::update() {
  clock.update();
  animations.update(clock.now);
}

auto animation_manager_ = std::make_shared<AnimationManager>();
export AnimationManager *animation_manager = animation_manager_.get();
