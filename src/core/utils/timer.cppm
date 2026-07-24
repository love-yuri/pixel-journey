//
// Created by love-yuri on 2026/7/24.
//

export module core.utils:timer;

import std;
import :thread_pool;
import yuri_log;

using namespace std::chrono;

namespace timer {

struct TimerNode {
  std::uint32_t id;                         // 节点 ID
  std::uint64_t ready_time;                 // 基于系统启动时间的就绪时间
  std::move_only_function<void()> function; // 待执行函数

  /**
   * 创建定时任务节点。
   *
   * @param node_id 节点 ID。
   * @param node_ready_time 节点就绪时间。
   * @param node_function 待执行函数。
   */
  TimerNode(
    std::uint32_t node_id,
    std::uint64_t node_ready_time,
    std::move_only_function<void()> node_function
  );

  /**
   * 按就绪时间和节点 ID 比较任务节点。
   *
   * @param node 待比较的任务节点。
   * @return 当前节点与目标节点的顺序关系。
   */
  [[nodiscard]] std::strong_ordering operator<=>(const TimerNode &node) const noexcept;
};

class Timer {
public:
  /** 创建定时器并启动任务线程。 */
  Timer();

  /** 停止任务线程并等待线程退出。 */
  virtual ~Timer();

  /**
   * 添加延迟执行任务。
   *
   * @tparam Fun 可调用对象类型。
   * @tparam Args 调用参数类型。
   * @param timeout_ms 延迟时间，单位为毫秒。
   * @param function 待执行的可调用对象。
   * @param args 传递给可调用对象的参数。
   */
  template <typename Fun, typename... Args>
    requires std::invocable<Fun, Args...>
  void timeout(std::uint64_t timeout_ms, Fun &&function, Args &&...args);

private:
  std::set<TimerNode> tasks_;         // 任务列表
  std::mutex mutex_;                  // 任务锁
  std::condition_variable cv_;        // 任务状态通知
  std::atomic_bool is_running = true; // 是否正在运行
  std::atomic_uint32_t id_ = 0;       // 下一个任务 ID
  std::thread thread_;                // 核心执行线程

  /** 持续提取并执行到期任务。 */
  void coreTask();
};

TimerNode::TimerNode(
  const std::uint32_t node_id,
  const std::uint64_t node_ready_time,
  std::move_only_function<void()> node_function
) : id(node_id), ready_time(node_ready_time), function(std::move(node_function)) {
}

std::strong_ordering TimerNode::operator<=>(const TimerNode &node) const noexcept {
  if (const auto result = ready_time <=> node.ready_time; result != 0) {
    return result;
  }
  return id <=> node.id;
}

Timer::Timer() : thread_(&Timer::coreTask, this) {
}

Timer::~Timer() {
  is_running = false;
  cv_.notify_one();
  if (thread_.joinable()) {
    thread_.join();
  }
}

template <typename Fun, typename... Args>
  requires std::invocable<Fun, Args...>
void Timer::timeout(const std::uint64_t timeout_ms, Fun &&function, Args &&...args) {
  {
    // clang-format off
    const auto current_time = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    TimerNode node {
      id_++,
      current_time + timeout_ms,
      [callable = std::forward<Fun>(function),
       ... arguments = std::forward<Args>(args)]() mutable {
        std::invoke(callable, arguments...);
      }
    };
    // clang-format on
    std::lock_guard lock(mutex_);
    tasks_.insert(std::move(node));
  }
  cv_.notify_one();
}

void Timer::coreTask() {
  while (true) {
    std::unique_lock lock(mutex_);
    cv_.wait(lock, [this] {
      return !is_running || !tasks_.empty();
    });

    if (!is_running) {
      return;
    }

    const auto begin = tasks_.begin();
    const auto current_time =
      duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    if (begin->ready_time <= current_time) {
      auto node_handle = tasks_.extract(begin);
      lock.unlock();
      thread_manager->addTask(std::move(node_handle.value().function));
    } else {
      cv_.wait_for(lock, milliseconds(begin->ready_time - current_time));
    }
  }
}

// 全局默认对象
Timer default_timer;

} // namespace timer

export namespace timer {

/**
 * 添加延迟执行任务。
 *
 * @tparam Fun 可调用对象类型。
 * @tparam Args 调用参数类型。
 * @param timeout_ms 延迟时间，单位为毫秒。
 * @param function 待执行的可调用对象。
 * @param args 传递给可调用对象的参数。
 */
template <typename Fun, typename... Args>
  requires std::invocable<Fun, Args...>
void timeout(const std::uint64_t timeout_ms, Fun &&function, Args &&...args) {
  default_timer.timeout(timeout_ms, std::forward<Fun>(function), std::forward<Args>(args)...);
}

}
