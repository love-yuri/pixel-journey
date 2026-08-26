//
// Created by Codex on 2026/8/24.
//

export module ui.runtime:dispatcher;

import std;

/**
 * UI 线程任务队列。
 *
 * 后台线程只负责投递任务，窗口渲染循环负责在 UI 线程执行任务。
 */
class Dispatcher {
public:
  /**
   * 投递一个需要在 UI 线程执行的任务。
   * @tparam F 可调用对象类型
   * @param task 待执行任务
   */
  template <typename F>
    requires std::invocable<F &>
  void post(F &&task) {
    std::lock_guard lock(mutex_);
    tasks_.emplace(std::forward<F>(task));
  }

  /** 执行当前已经投递的全部 UI 任务。 */
  void processPending();

private:
  std::mutex mutex_{};                                  // 保护任务队列
  std::queue<std::move_only_function<void()>> tasks_{}; // 待执行的 UI 任务
};

void Dispatcher::processPending() {
  std::queue<std::move_only_function<void()>> pending;
  {
    std::lock_guard lock(mutex_);
    pending.swap(tasks_);
  }

  while (!pending.empty()) {
    auto task = std::move(pending.front());
    pending.pop();
    task();
  }
}

export namespace ui {
  Dispatcher dispatcher;
}