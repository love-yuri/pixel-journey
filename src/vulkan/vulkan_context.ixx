/**
 * vulkan上下文模块
 */
export module vulkan.context;

import vulkan.api;
import vulkan.detail;
import common_config;
import vulkan_config;
import glfw.api;
import std;

using namespace vk;

/**
 * 每帧渲染数据
 */
struct render_frame {
  Semaphore image_available;                 // 图像获取完成的信号
  Semaphore render_finished;                 // 渲染完成的信号
  Fence fence;                               // 帧渲染完成的栅栏
  std::vector<CommandBuffer> command_buffers; // 专属命令缓冲区
};

/**
 * 全局vulkan上下文
 */
class vulkan_context final {
  glfw::application app;                   // glfw app
  DebugUtilsMessengerEXT debug_messenger_; // debug_messenger

public:
  Instance instance;                       // vulkan实例
  PhysicalDevice physical_device;          // 物理 gpu设备
  Device logic_device;                     // 逻辑 gpu设备
  Queue queue;                             // gpu渲染队列
  std::uint32_t queue_family_index = 0;    // 选择的index
  CommandPool command_pool;                // command pool
  std::vector<render_frame> render_frames; // cmd buffers

  vulkan_context();
  ~vulkan_context() = default;

private:
  /**
   * 初始化vulkan_instance
   */
  void init_instance();
};

vulkan_context::vulkan_context() :instance(nullptr) {
  // 创建instance
  init_instance();

  // 获取可用 gpu 设备
  auto [physical_device, index] = pick_physical_device_and_graphics_queue(instance);

  // 设置可用gpu设备和index
  this->queue_family_index = index;
  this->physical_device = physical_device;

  // 创建逻辑设备
  this->logic_device = create_logical_device(physical_device, index);

  // 创建queue
  this->queue = this->logic_device.getQueue(index, index);

  // 创建command pool
  this->command_pool = check_vk_result(
    logic_device.createCommandPool( {
      CommandPoolCreateFlagBits::eResetCommandBuffer,
      index
    }),
    "创建 Command Pool"
  );
}

void vulkan_context::init_instance() {
  const detail::instance_create_info info;
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
