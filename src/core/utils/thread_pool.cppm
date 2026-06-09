//
// Created by love-yuri on 2026/4/28.
//

export module thread_pool;

import std;
import yuri_log;

class ThreadPool {
  std::atomic_bool stopped{false};
  std::once_flag init_flag;
  std::once_flag shutdown_flag;
  std::mutex mutex;
  std::condition_variable cv;
  std::queue<std::function<void()>> tasks;
  std::vector<std::thread> threads;

public:
  explicit ThreadPool(const int n = 5) : threads(n) {}

  ThreadPool(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;

  ~ThreadPool() {
    shutDown();
  }

  void init() {
    std::call_once(init_flag, [this] {
      for (int i = 0; i < static_cast<int>(threads.size()); i++) {
        threads[i] = std::thread([this, id = i + 1] {
          std::function<void()> task;
          while (true) {
            {
              std::unique_lock lock(mutex);
              cv.wait(lock, [this] {
                return stopped.load(std::memory_order_relaxed) || !tasks.empty();
              });

              if (stopped.load(std::memory_order_relaxed) && tasks.empty()) {
                return;
              }

              task = std::move(tasks.front());
              tasks.pop();
            }
            task();
          }
        });
      }
    });
  }

  template <typename Fun, typename... Args>
  auto addTask(Fun &&fun, Args &&...args) {
    init();

    using return_type = std::invoke_result_t<Fun, Args...>;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
      [f = std::forward<Fun>(fun), ... a = std::forward<Args>(args)] {
        return std::invoke(f, a...);
      });

    {
      std::lock_guard lock(mutex);
      tasks.emplace([task] { (*task)(); });
    }

    cv.notify_one();
    return task->get_future();
  }

private:
  void shutDown() {
    std::call_once(shutdown_flag, [this] {
      {
        std::lock_guard lock(mutex);
        stopped.store(true, std::memory_order_relaxed);
      }
      cv.notify_all();
      for (auto &thread : threads) {
        if (thread.joinable()) {
          thread.join();
        }
      }
    });
  }
};

export auto thread_manager = std::make_shared<ThreadPool>(5);
