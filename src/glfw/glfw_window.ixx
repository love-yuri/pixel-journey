export module glfw.window;

import glfw.api;
import glfw.context;
import vulkan.detail;
import vulkan.api;
import vulkan.context;
import vulkan_config;
import yuri_log;
import common_config;
import std;

export namespace glfw {

/**
 * 默认使用以下两个，大部分设备都支持
 * 默认不会进行检查
 */
constexpr auto default_surface_format = vk::Format::eB8G8R8A8Unorm;             // 默认format
constexpr auto default_surface_color_space = vk::ColorSpaceKHR::eSrgbNonlinear; // 默认color_space

class glfw_window {
public:
  int m_width;                // 窗口宽度
  int m_height;               // 窗口高度
  GLFWwindow *m_window{};     // 窗口指针
  std::string m_title{};      // 窗口标题
  glfw_context context{};     // context

public:
  glfw_window(int width, int height, std::string_view title = "yuri");
  virtual ~glfw_window();

  /**
   * 获取窗口宽度
   */
  [[nodiscard]] int width() const;

  /**
   * 获取窗口高度
   */
  [[nodiscard]] int height() const;

  /**
   * 返回buffer的宽高
   * @return 0: 宽度 ：1、高度
   */
  [[nodiscard]] vk::Extent2D get_buffer_size() const;

  /**
   * 获取一个可用的frame
   * @return fram
   */
  [[nodiscard]] std::uint32_t acquire_frame(std::uint32_t index) const;

  /**
   * 展示debug信息
   */
  void show_debug_info() const;

private:
  /**
   * 创建surface
   */
  [[nodiscard]] vk::VkSurfaceKHR create_surface() const;

  /**
   * 创建交换链
   */
  [[nodiscard]] vk::SwapchainKHR create_swapchain();

  /**
   * 初始化context
   */
  void init_context();
};

glfw_window::glfw_window(const int width, const int height, const std::string_view title) :
  m_width(width),
  m_height(height),
  m_title(title),
  m_window(create_window(width, height, title)) {

  // 初始化context
  init_context();

  // 打印debug信息
  if constexpr (is_debug_mode) {
    show_debug_info();
  }
}

glfw_window::~glfw_window() {
  destroy_window(m_window);
}

vk::VkSurfaceKHR glfw_window::create_surface() const {
  vk::VkSurfaceKHR surface{};
  glfwCreateWindowSurface(vulkan_context->instance, m_window, nullptr, &surface);
  return surface;
}

vk::SwapchainKHR glfw_window::create_swapchain() {
  const auto caps = context.capabilities;
  context.image_count = caps.minImageCount + 1;
  const vk::detail::swapchain_create_info info {
    context.surface,
    context.image_count,
    default_surface_format,
    default_surface_color_space,
    get_buffer_size(),
    caps.currentTransform
  };

  return vk::check_vk_result (
    vulkan_context->logic_device.createSwapchainKHR(info),
    "创建swapchain"
  );
}

void glfw_window::init_context() {
  context.surface = create_surface();
  context.window = m_window;
  context.capabilities = vk::check_vk_result(
    vulkan_context->physical_device.getSurfaceCapabilitiesKHR(context.surface),
    "获取 Surface Capabilities"
  );
  context.swapchain = create_swapchain();
  context.images = vk::check_vk_result(
     vulkan_context->logic_device.getSwapchainImagesKHR(context.swapchain),
     "获取 Swap Images"
   );

  // 创建渲染帧
  context.frames = std::vector<render_frame>(context.image_count);
  for (auto i = 0; i < context.image_count; ++i) {
    auto &[render_finished, image_available, fence, command_buffers] = context.frames[i];
    const auto &logic_device = vulkan_context->logic_device;

    // 创建command buffers
    command_buffers = std::vector<vk::CommandBuffer>(vk::command_buffer_size);
    const vk::CommandBufferAllocateInfo allocInfo {
      vulkan_context->command_pool,
      vk::CommandBufferLevel::ePrimary,
      vk::command_buffer_size
    };
    vk::check_vk_result(logic_device.allocateCommandBuffers(&allocInfo, command_buffers.data()),"分配 Command Buffers");

    // 创建fence
    constexpr vk::FenceCreateInfo fence_create_info = { vk::FenceCreateFlagBits::eSignaled };
    vk::check_vk_result(
      logic_device.createFence(&fence_create_info, nullptr, &fence),
      "创建 Fence"
    );

    // 信号量
    constexpr vk::SemaphoreCreateInfo semaphore_create_info;
    vk::check_vk_result(
      logic_device.createSemaphore(&semaphore_create_info, nullptr, &image_available),
      "创建 semaphore"
    );
    vk::check_vk_result(
      logic_device.createSemaphore(&semaphore_create_info, nullptr, &render_finished),
      "创建 semaphore"
    );
  }
}

int glfw_window::width() const {
  return m_width;
}

int glfw_window::height() const {
  return m_height;
}

vk::Extent2D glfw_window::get_buffer_size() const {
  int height, width;
  glfwGetFramebufferSize(m_window, &width, &height);
  return {
    static_cast<std::uint32_t>(width),
    static_cast<std::uint32_t>(height)
  };
}

std::uint32_t glfw_window::acquire_frame(const std::uint32_t index) const {
  std::uint32_t imageIndex;
  vk::check_vk_result(vulkan_context->logic_device.acquireNextImageKHR(
    context.swapchain, std::numeric_limits<std::uint64_t>::max(),
    context.frames[index].image_available, nullptr, &imageIndex
  ), "acquire image index");

  return imageIndex;
}

void glfw_window::show_debug_info() const {
  auto caps = context.capabilities;
  yuri::info("Surface capabilities:");
  yuri::info("  minImageCount: {}", caps.minImageCount);
  yuri::info("  maxImageCount: {}", caps.maxImageCount);
  yuri::info("  currentExtent: {} x {}", caps.currentExtent.width, caps.currentExtent.height);

  const auto formats = vulkan_context->physical_device.getSurfaceFormatsKHR(context.surface);
  auto res = vk::check_surface_format_support(formats.value, default_surface_format, default_surface_color_space);
  yuri::info("format: {}, space_khr: {} 支持情况 -> {}", vk::to_string(default_surface_format), vk::to_string(default_surface_color_space), res);
}

} // namespace glfw