//
// Created by yuri on 2026/1/19.
//

export module ui.animation:animation_manager;

import :clock;
import :linear_animation;
import std;

using namespace ui::animation;

export namespace ui::animation {

class AnimationManager {
  FrameClock clock; // 动画计时clock
  std::vector<LinearAnimation<float>*> animations {};

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
};

void AnimationManager::start(const float from, const float to, const float duration, float *value) {
  *value = from;
  const auto animation = new LinearAnimation<float>(clock.now);
  animation->from = from;
  animation->to = to;
  animation->duration = duration;
  animation->value = value;
  animations.push_back(animation);
}

} // namespace ui::animation

bool operator==(const std::vector<LinearAnimation<float> *>::iterator & it, const std::vector<LinearAnimation<float> *>::iterator & end) {
  return it.base() == end.base();
}

void AnimationManager::update() {
  clock.update();
  for (auto it = animations.begin(); it != animations.end();) {
    if (!(*it)->update(clock.now)) {
      delete *it;
      it = animations.erase(it);
    } else {
      ++it;
    }
  }
}

auto animation_manager_ = std::make_shared<AnimationManager>();
export AnimationManager *animation_manager = animation_manager_.get();
