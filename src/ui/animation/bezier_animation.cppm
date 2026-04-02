//
// Created by love-yuri on 2026/4/2.
//

export module ui.animation:bezier_animation;

import :core;
import skia;
import core;
import ui.algorithm;
import std;

using namespace profiling;
using namespace ui::algorithm;
using namespace skia;

export namespace ui::animation {

/**
 * 三次贝塞尔曲线
 * 通过控制点 (x1, y1), (x2, y2) 定义缓动曲线
 * 起点 (0,0) 终点 (1,1) 固定
 */
class CubicBezier {
public:
  float x1, y1, x2, y2;

  constexpr CubicBezier(float x1, float y1, float x2, float y2) noexcept;

  /**
   * 给定输入 t (0~1) 计算缓动后的值
   */
  [[nodiscard]] float solve(float t) const noexcept;

  /** 线性: 无缓动 */
  static constexpr CubicBezier Linear();
  /** 缓入缓出: 默认过渡效果 */
  static constexpr CubicBezier Ease();
  /** 缓入: 慢起快停 */
  static constexpr CubicBezier EaseIn();
  /** 缓出: 快起慢停 */
  static constexpr CubicBezier EaseOut();
  /** 缓入缓出: 两端慢中间快 */
  static constexpr CubicBezier EaseInOut();

private:
  /** 采样曲线 x 分量 */
  [[nodiscard]] float sampleCurveX(float u) const noexcept;

  /** Newton-Raphson 求解: 已知 x=t 反求参数 u */
  [[nodiscard]] float sampleX(float t) const noexcept;
};

/**
 * 贝塞尔曲线动画
 * 在线性插值基础上, 将时间参数 t 经过贝塞尔曲线变换后再插值
 */
template <CanLerp T>
class BezierAnimation : public IAnimation {
public:
  using Setter = function_ref<void(const T &)>;

  BezierAnimation(const T &from,const T &to,float duration, CubicBezier curve, Setter setter) noexcept;

  /**
   * 更新动画帧
   * @return 动画是否已完成
   */
  bool update(std::uint64_t now) override;

private:
  T from;              // 起始值
  T to;                // 目标值
  float inv_dur;       // 间隔比例 1 / (duration * 1000.0)
  std::uint64_t start; // 起始时间
  CubicBezier curve;   // 贝塞尔缓动曲线
  Setter setter;       // setter回调
};

constexpr CubicBezier::CubicBezier(const float x1,
                                   const float y1,
                                   const float x2,
                                   const float y2) noexcept : x1(x1), y1(y1), x2(x2), y2(y2) {
}

constexpr CubicBezier CubicBezier::Linear() {
  return { 0.f, 0.f, 1.f, 1.f };
}
constexpr CubicBezier CubicBezier::Ease() {
  return { 0.25f, 0.1f, 0.25f, 1.f };
}
constexpr CubicBezier CubicBezier::EaseIn() {
  return { 0.42f, 0.f, 1.f, 1.f };
}
constexpr CubicBezier CubicBezier::EaseOut() {
  return { 0.f, 0.f, 0.58f, 1.f };
}
constexpr CubicBezier CubicBezier::EaseInOut() {
  return { 0.42f, 0.f, 0.58f, 1.f };
}

float CubicBezier::solve(const float t) const noexcept {
  const float u = sampleX(t);
  const float a = 1.f - u;
  return 3.f * a * a * u * y1 + 3.f * a * u * u * y2 + u * u * u;
}

float CubicBezier::sampleCurveX(const float u) const noexcept {
  const float a = 1.f - u;
  return 3.f * a * a * u * x1 + 3.f * a * u * u * x2 + u * u * u;
}

float CubicBezier::sampleX(const float t) const noexcept {
  float u = t;
  for (int i = 0; i < 8; ++i) {
    const float err = sampleCurveX(u) - t;
    if (std::abs(err) < 1e-7f) break;
    const float dx =
      3.f * (1.f - u) * (1.f - u) * x1 + 6.f * (1.f - u) * u * (x2 - x1) + 3.f * u * u * (1.f - x2);
    if (std::abs(dx) < 1e-7f) break;
    u -= err / dx;
  }
  return std::clamp(u, 0.f, 1.f);
}

template <CanLerp T>
BezierAnimation<T>::BezierAnimation(const T &from,
                                    const T &to,
                                    const float duration,
                                    const CubicBezier curve,
                                    Setter setter) noexcept :
  from(from), to(to), inv_dur(1.f / (duration * 1000.f)), start(frame_clock.now), curve(curve),
  setter(std::move(setter)) {
}

template <CanLerp T>
bool BezierAnimation<T>::update(const std::uint64_t now) {
  const float t = static_cast<float>(now - start) * inv_dur;
  if (t >= 1.f) {
    setter(to);
    return true;
  }
  setter(lerp(from, to, curve.solve(t)));
  return false;
}

} // namespace ui::animation
