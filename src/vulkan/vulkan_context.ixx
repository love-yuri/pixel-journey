/**
 * vulkan上下文模块
 */
export module vulkan.context;

import vulkan.api;
import vulkan.detail;
import yuri_log;
import skia.api;
import configuration;
import glfw.api;
import std;

using namespace vk;
using namespace skia;

/**
 * 全局vulkan上下文
 */
class vulkan_context final {
  glfw::application app;                   // glfw app
  DebugUtilsMessengerEXT debug_messenger_; // debug_messenger

public:
  Instance instance;                               // vulkan实例
  PhysicalDevice physical_device;                  // 物理 gpu设备
  Device logic_device;                             // 逻辑 gpu设备
  Queue queue;                                     // gpu渲染队列
  std::uint32_t queue_family_index = 0;            // 选择的index
  CommandPool command_pool;                        // command pool
  DispatchLoaderDynamic instance_dynamic_dispatch; // instance 动态加载器
  gpu::VulkanBackendContext skia_vk_context;     // vk_context
  sk_sp<GrDirectContext> skia_direct_context;      // GrDirectContext
  gpu::MutableTextureState present_state;  // skia present s

  vulkan_context();
  ~vulkan_context();

  /**
   * 分配command_buffer
   * @return 分配好的command_buffer
   */
  [[nodiscard]] std::vector<CommandBuffer> allocate_command_buffer() const;

  /**
   * 创建fence
   * @return Fence
   */
  [[nodiscard]] Fence create_fence() const;

  /**
   * 创建Semaphore
   * @return Semaphore
   */
  [[nodiscard]] Semaphore create_semaphore() const;

  /**
   * 获取 SurfaceCapabilitiesKHR
   * @return SurfaceCapabilitiesKHR
   */
  [[nodiscard]] SurfaceCapabilitiesKHR get_surface_capabilities(const SurfaceKHR &) const;

  /**
   * 获取 交换链里的image
   * @return std::vector<Image>
   */
  [[nodiscard]] std::vector<Image> get_images(const SwapchainKHR &) const;

private:
  /**
   * 初始化vulkan_instance
   */
  void init_instance();
};

vulkan_context::vulkan_context() : instance(nullptr) {
  // 创建instance
  init_instance();

  // 获取可用 gpu 设备
  auto [physical_device, index] = pick_physical_device_and_graphics_queue(instance);

  // 设置可用gpu设备和index
  this->queue_family_index = index;
  this->physical_device = physical_device;

  // 创建逻辑设备
  this->logic_device = create_logical_device(physical_device, queue_family_index);

  // 创建queue
  this->queue = this->logic_device.getQueue(queue_family_index, queue_family_index);

  // 创建command pool
  this->command_pool = check_vk_result(
    logic_device.createCommandPool( {
      CommandPoolCreateFlagBits::eResetCommandBuffer,
      queue_family_index
    }),
    "创建 Command Pool"
  );

  // 创建skia context
  skia_vk_context.fInstance = instance;
  skia_vk_context.fPhysicalDevice = physical_device;
  skia_vk_context.fDevice = logic_device;
  skia_vk_context.fGraphicsQueueIndex = queue_family_index;
  skia_vk_context.fQueue = queue;
  skia_vk_context.fGetProc = vulkan_get_proc;
  skia_direct_context = MakeVulkan(skia_vk_context);
  present_state = gpu::MutableTextureStates::MakeVulkan(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, queue_family_index);
}

vulkan_context::~vulkan_context() {
  skia_direct_context.reset();
  logic_device.destroyCommandPool(command_pool);
  logic_device.destroy();
  if constexpr (is_debug_mode) {
    instance.destroyDebugUtilsMessengerEXT(debug_messenger_, nullptr, instance_dynamic_dispatch);
  }
  instance.destroy();
}

std::vector<CommandBuffer> vulkan_context::allocate_command_buffer() const {
  auto command_buffers = std::vector<CommandBuffer>(command_buffer_size);
  const CommandBufferAllocateInfo allocInfo {
    command_pool,
    CommandBufferLevel::ePrimary,
    command_buffer_size
  };
  check_vk_result(logic_device.allocateCommandBuffers(&allocInfo, command_buffers.data()),"分配 Command Buffers");
  return command_buffers;
}

Fence vulkan_context::create_fence() const {
  Fence fence;
  constexpr FenceCreateInfo fence_create_info = {
    FenceCreateFlagBits::eSignaled
  };
  check_vk_result(
    logic_device.createFence(&fence_create_info, nullptr, &fence),
    "创建 Fence"
  );
  return fence;
}

Semaphore vulkan_context::create_semaphore() const {
  Semaphore semaphore;
  constexpr SemaphoreCreateInfo semaphore_create_info;
  check_vk_result(
    logic_device.createSemaphore(&semaphore_create_info, nullptr, &semaphore),
    "创建 semaphore"
  );
  return semaphore;
}

SurfaceCapabilitiesKHR vulkan_context::get_surface_capabilities(const SurfaceKHR &surface) const {
  return check_vk_result (
    physical_device.getSurfaceCapabilitiesKHR(surface),
    "获取 Surface Capabilities"
  );
}

std::vector<Image> vulkan_context::get_images(const SwapchainKHR & swapchain) const {
  return check_vk_result (
    logic_device.getSwapchainImagesKHR(swapchain),
    "获取 Swap Images"
  );
}

void vulkan_context::init_instance() {
  const detail::instance_create_info info;
  if (const auto res = createInstance(&info, nullptr, &instance); res != Result::eSuccess) {
    throw std::runtime_error(std::format("创建vulkan_instance 失败, 错误码: {}", to_string(res)));
  }

  // 初始化加载器
  if constexpr (is_debug_mode) {
    instance_dynamic_dispatch = DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
    const DebugUtilsMessengerCreateInfoEXT createInfo{
      {},
      DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | DebugUtilsMessageSeverityFlagBitsEXT::eWarning | DebugUtilsMessageSeverityFlagBitsEXT::eError,
      DebugUtilsMessageTypeFlagBitsEXT::eGeneral | DebugUtilsMessageTypeFlagBitsEXT::eValidation | DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
      reinterpret_cast<PFN_DebugUtilsMessengerCallbackEXT>(debugCallback),
      nullptr,
    };

    debug_messenger_ = check_vk_result(
      instance.createDebugUtilsMessengerEXT(createInfo, nullptr, instance_dynamic_dispatch),
      "Debug 附加消息"
    );
  }
}

// 初始全局context
auto vulkan_context_ = std::make_shared<vulkan_context>();
export vulkan_context *vulkan_context = vulkan_context_.get();
