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
  context.capabilities = vulkan_context->get_surface_capabilities(context.surface);
  context.swapchain = create_swapchain();
  context.images = vulkan_context->get_images(context.swapchain);

  // 创建渲染帧
  context.fences.resize(context.image_count);
  context.render_finished_semaphores.resize(context.image_count);
  context.image_available_semaphores.resize(context.image_count);
  context.command_buffers.resize(context.image_count);
  for (auto i = 0; i < context.image_count; ++i) {
    context.render_finished_semaphores[i] = vulkan_context->create_semaphore();
    context.image_available_semaphores[i] = vulkan_context->create_semaphore();
    context.fences[i] = vulkan_context->create_fence();
    context.command_buffers[i] = vulkan_context->allocate_command_buffer();
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