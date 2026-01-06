/**
 * vulkan上下文模块
 */
export module vulkan.context;

import vulkan.api;
import vulkan.detail;
import common_config;
import glfw.api;
import std;

using namespace vk;

/**
 * 全局vulkan上下文
 */
class vulkan_context final {
  glfw::application app;                   // glfw app
  DebugUtilsMessengerEXT debug_messenger_; // debug_messenger
  const detail::instance_create_info info;

public:
  Instance instance;
  PhysicalDevice physical_device;
  Device logic_device;
  Queue queue;

  vulkan_context();
  ~vulkan_context() = default;

private:
  /**
   * 初始化vulkan_instance
   */
  void init_instance();
};

vulkan_context::vulkan_context():instance(nullptr) {
  init_instance();
  auto [physical_device, index] = pick_physical_device_and_graphics_queue(instance);
  this->physical_device = physical_device;
  this->logic_device = create_logical_device(physical_device, index);
  this->queue = this->logic_device.getQueue(index, index);
}

void vulkan_context::init_instance() {
  if (const auto res = createInstance(&info, nullptr, &instance); res != Result::eSuccess) {
    throw std::runtime_error(std::format("创建vulkan_instance 失败, 错误码: {}", to_string(res)));
  }

  if constexpr (is_debug_mode) {
    const DebugUtilsMessengerCreateInfoEXT createInfo{
      {},
      DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | DebugUtilsMessageSeverityFlagBitsEXT::eWarning | DebugUtilsMessageSeverityFlagBitsEXT::eError,
      DebugUtilsMessageTypeFlagBitsEXT::eGeneral | DebugUtilsMessageTypeFlagBitsEXT::eValidation | DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
      reinterpret_cast<PFN_DebugUtilsMessengerCallbackEXT>(debugCallback),
      nullptr,
    };

    const auto dispatcher = DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
    const auto res = instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dispatcher);
    check_vk_result(res.result, "Debug 附加消息");
    debug_messenger_ = res.value;
  }
}

// 初始全局context
auto vulkan_context_ = std::make_shared<vulkan_context>();
export vulkan_context *vulkan_context = vulkan_context_.get();
