//
// Created by yuri on 2026/1/23.
//

export module signal;

import std;
import yuri_log;

export template<typename>
class function_ref;

export template <typename R, typename... Args>
class function_ref<R(Args...)> {
  function_ref() = default;
public:
  // 对象 + 成员函数或闭包对象调用
  using member_invoke_fn = R (*)(void *, Args&&...);

  // 普通函数或无捕获 lambda 调用
  using free_invoke_fn = R (*)(Args&&...);

  /**
   * 通过函数指针初始化
   * @param func 函数指针
   */
  explicit function_ref(const free_invoke_fn func) noexcept : free_fn(func) {
  }

  /**
   * 默认构造函数
   * 自行处理指针
   */
  function_ref(void *obj, member_invoke_fn method) noexcept : object_ptr(obj), member_fn(method) {
  }

  /**
   * 通过成员函数造
   * @tparam T 类类型
   * @tparam ptr 成员函数
   * @param obj 成员实例指针
   * @return
   */
  template <typename T, auto ptr>
  static function_ref from(T *obj) noexcept {
    const auto func = [](void *this_, Args &&...args) -> R {
      return (static_cast<T *>(this_)->*ptr)(std::forward<Args>(args)...);
    };
    return {obj, func};
  }

  /**
   * 通过可调用对象构造：lambda或者普通函数
   * @tparam F 可调用对象类型
   * @param f 可调用对象/函数
   * @return
   */
  template <typename F>
  requires std::invocable<F &, Args...>
  static function_ref from(F &f) {
    if constexpr (std::is_function_v<std::remove_reference_t<F>>) {
      return function_ref(f);
    } else {
      return function_ref(
        std::addressof(f),
        [](void *p, Args&& ...args) -> R {
          return (*static_cast<F *>(p))(std::forward<Args>(args)...);
        }
      );
    }
  }

  R operator()(Args&&... args) const {
    if (object_ptr != nullptr) {
      return member_fn(object_ptr, std::forward<Args>(args)...);
    }
    return free_fn(std::forward<Args>(args)...);
  }

private:
  void *object_ptr = nullptr;           // 指向对象或闭包（有捕获 lambda 或成员函数）
  member_invoke_fn member_fn = nullptr; // 对象/成员函数调用
  free_invoke_fn free_fn = nullptr;     // 普通函数或无捕获 lambda 调用
};

export template <typename ...Args>
class Signal {
  using SignalType = function_ref<void(Args...)>;
  std::vector<SignalType> slots{};

public:

  /**
   * 创建信号连接
   * @param obj this
   */
  template <typename T, auto ptr>
  inline void connect(T* obj) noexcept {
    slots.emplace_back(SignalType::template from<T, ptr>(obj));
  }

  /**
   * 创建信号连接
   */
  template <typename F>
  requires std::invocable<F &, Args...>
  inline void connect(F& f) {
    slots.emplace_back(SignalType::from(f));
  }

  /**
   * 发送信号
   * @param args 参数
   */
  inline void emit(Args&&... args) {
    for (auto & slot: slots) {
      slot(std::forward<Args>(args)...);
    }
  }
};

