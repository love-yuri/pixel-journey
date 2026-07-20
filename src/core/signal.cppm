//
// Created by yuri on 2026/1/23.
//

export module core:signal;

import std;
import yuri_log;

export template <typename>
class function_ref;

export template <typename R, typename... Args>
class function_ref<R(Args...)> {
public:
  // 函数类型
  using InvokeFunType = R (*)(void *, Args &&...);
  function_ref() = delete;

  /**
   * 默认构造函数
   * 自行处理指针
   */
  function_ref(void *obj, const InvokeFunType func) noexcept : object_ptr(obj), invoke_fun(func) {
  }

  /**
   * 通过成员函数造
   * @tparam T 类类型
   * @tparam ptr 成员函数
   * @param obj 成员实例指针
   * @return
   */
  template <auto ptr, typename T>
  static function_ref from(T *obj) noexcept {
    static_assert(ptr != nullptr, "ptr cannot be null");
    const auto func = [](void *this_, Args &&...args) -> R {
      return (static_cast<T *>(this_)->*ptr)(std::forward<Args>(args)...);
    };
    return { obj, func };
  }

  /**
   * 通过可调用对象构造：lambda或者普通函数
   * @tparam F 可调用对象类型
   * @param f 可调用对象/函数
   * @return
   */
  template <typename F>
    requires std::invocable<F &, Args...>
  static function_ref from(F &f) noexcept {
    return function_ref(std::addressof(f), [](void *p, Args &&...args) -> R {
      return (*static_cast<F *>(p))(std::forward<Args>(args)...);
    });
  }

  R operator()(Args &&...args) const {
    return invoke_fun(object_ptr, std::forward<Args>(args)...);
  }

  /** 获取目标对象指针 */
  [[nodiscard]] void *target_object() const noexcept {
    return object_ptr;
  }

  /** 获取目标函数指针 */
  [[nodiscard]] InvokeFunType target_function() const noexcept {
    return invoke_fun;
  }

  /** 三元运算符 */
  auto operator<=>(const function_ref &ref) const {
    if (auto cmp = object_ptr <=> ref.object_ptr; cmp != 0) {
      return cmp;
    }
    return invoke_fun <=> ref.invoke_fun;
  }

private:
  void *object_ptr = nullptr;         // 指向对象或闭包
  InvokeFunType invoke_fun = nullptr; // 对象/成员函数调用
};

export template <typename... Args>
class Signal {
public:
  /**
   * 创建信号连接
   * @param obj this
   */
  template <auto ptr, typename T>
  inline void connect(T *obj) noexcept {
    slots.emplace_back(SignalType::template from<ptr, T>(obj));
  }

  /**
   * 创建信号连接
   */
  template <typename F>
    requires std::invocable<F &, Args...>
  inline void connect(F &f) noexcept {
    slots.emplace_back(SignalType::from(f));
  }

  /**
   * 断开链接
   */
  template <typename T>
  inline void disconnect(T *obj) noexcept {
    slots.erase(
      std::remove_if(
        slots.begin(), slots.end(), [obj](auto slot) { return slot.target_object() == obj; }
      ),
      slots.end()
    );
  }

  /**
   * 创建信号连接
   * 可以传临时lambda 但是仅限测试
   */
  template <typename F>
    requires std::invocable<F &, Args...>
  inline void connect(F &&f) noexcept {
    slots.emplace_back(SignalType::from(f));
  }

  /**
   * 发送信号
   * @param args 参数
   */
  inline void emit(Args... args) {
    for (auto &slot : slots) {
      slot(std::forward<Args>(args)...);
    }
  }

private:
  using SignalType = function_ref<void(Args...)>; // 信号类型
  std::vector<SignalType> slots{};                // 信号列表
};
