//
// Created by love-yuri on 2026/8/31.
//

export module yuri.core:service_collection;

import std;

class ServiceCollection {

public:
  ServiceCollection(const ServiceCollection&) = delete;
  ServiceCollection& operator=(const ServiceCollection&) = delete;
  ServiceCollection(ServiceCollection&&) = delete;
  ServiceCollection& operator=(ServiceCollection&&) = delete;

  ServiceCollection() = default;
  virtual ~ServiceCollection() = default;

  /**
   * 从容器库中获取指定服务 如果没有则抛出异常
   * @tparam T 服务类型
   * @return 返回获取到的服务
   */
  template <typename T>
  T* getRequiredService() {
    if (auto* service = getService<T>()) {
      return service;
    }

    auto &type = typeid(T);
    throw std::runtime_error(std::format("No injector: {} found", type.name()));
  }

  /**
   * 从容器库中获取指定服务 获取失败返回nullptr
   * @tparam T 服务类型
   * @return 返回获取到的服务
   */
  template <typename T>
  T* getService() {
    const auto type = std::type_index(typeid(T));
    std::lock_guard lock(mutex_);
    if (const auto iter = injector_.find(type); iter != injector_.end()) {
      if (iter->second.pointer == nullptr) {
        iter->second.pointer = new T();
      }
      return static_cast<T*>(iter->second.pointer);
    }

    return nullptr;
  }

  /**
   * 向容器中注入单例服务
   */
  template <typename T>
  void addSingleton() {
    const auto type = std::type_index(typeid(T));
    std::lock_guard lock(mutex_);
    if (const auto iter = injector_.find(type); iter != injector_.end()) {
      return;
    }

    // clang-format off
    injector_.try_emplace(type,
      nullptr, [](void* p) {
        delete static_cast<T*>(p);
      }
    );
    // clang-format on
  }

  /**
   * 向容器中注入单例服务
   * 该声明周期将由外部接管而不是由本容器处理
   */
  template <typename T>
  void addSingleton(T *pointer) {
    const auto type = std::type_index(typeid(T));
    std::lock_guard lock(mutex_);
    if (const auto iter = injector_.find(type); iter != injector_.end()) {
      return;
    }

    injector_.try_emplace(type, pointer, nullptr);
  }

  /**
   * 移除注册服务
   */
  template <typename T>
  void removeService() {
    std::lock_guard lock(mutex_);
    injector_.erase(std::type_index(typeid(T)));
  }

private:
  struct IocEntity {
    void* pointer;          // 实例指针
    void (*destroy)(void*); // 销毁函数

    ~IocEntity() {
      if (destroy && pointer) {
        destroy(pointer);
        destroy = nullptr;
        pointer = nullptr;
      }
    }
  };

  std::unordered_map<std::type_index, IocEntity> injector_{};  // 注入容器
  std::mutex mutex_;                                           // 服务锁
};

// 导出全局注射器
export ServiceCollection injector;