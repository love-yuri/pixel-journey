//
// Created by yuri on 2026/1/19.
//
export module ui.animation:clock;

import std;

using namespace std::chrono;
using namespace std::chrono_literals;
using duration = steady_clock::duration;

export namespace ui::animation {

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
