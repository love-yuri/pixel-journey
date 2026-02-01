//
// Created by yuri on 2026/1/31.
//

export module profiling:fps_counter;

import std;

export namespace profiling {

/**
 * fps 统计工具
 */
class FpsCounter {
  int frameCount = 0;
  double lastTime = 0.0;
  double fps = 0.0;

public:
  void update() {
    // 获取当前时间（秒，高精度）
    const auto now = std::chrono::high_resolution_clock::now();
    const auto duration = now.time_since_epoch();
    const auto currentTime = std::chrono::duration<double>(duration).count();

    frameCount++;

    // 每秒钟计算一次FPS
    if (currentTime - lastTime >= 1.0) {
      fps = frameCount / (currentTime - lastTime);
      frameCount = 0;
      lastTime = currentTime;
    }
  }

  [[nodiscard]] double getFPS() const {
    return fps;
  }
};

} // namespace profiling