export module glfw.window;

import glfw.api;
import vulkan.api;
import vulkan.instance;
import vulkan.context;
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
  vk::SurfaceKHR m_surface{}; // surface

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
  [[nodiscard]] std::tuple<std::uint32_t, std::uint32_t> get_buffer_size() const;

  /**
   * 展示debug信息
   */
  void show_debug_info() const;

private:
  /**
   * 创建surface
   */
  [[nodiscard]] vk::VkSurfaceKHR create_surface() const;
};

glfw_window::glfw_window(const int width, const int height, const std::string_view title) :
  m_width(width),
  m_height(height),
  m_title(title),
  m_window(create_window(width, height, title)), m_surface(create_surface()) {

  if constexpr (is_debug_mode) {
    show_debug_info();
  }
}

glfw_window::~glfw_window() {
  destroy_window(m_window);
}

vk::VkSurfaceKHR glfw_window::create_surface() const {
  vk::VkSurfaceKHR surface{};
  glfwCreateWindowSurface(*global_vulkan_instance, m_window, nullptr, &surface);
  return surface;
}

int glfw_window::width() const {
  return m_width;
}

int glfw_window::height() const {
  return m_height;
}

std::tuple<std::uint32_t, std::uint32_t> glfw_window::get_buffer_size() const {
  int height, width;
  glfwGetFramebufferSize(m_window, &width, &height);
  return { width, height };
}

void glfw_window::show_debug_info() const {
  const auto physical_device = global_vulkan_instance->physical_device;
  auto caps = physical_device.getSurfaceCapabilitiesKHR(m_surface).value;
  yuri::info("Surface capabilities:");
  yuri::info("  minImageCount: {}", caps.minImageCount);
  yuri::info("  maxImageCount: {}", caps.maxImageCount);
  yuri::info("  currentExtent: {} x {}", caps.currentExtent.width, caps.currentExtent.height);

  const auto formats = global_vulkan_instance->physical_device.getSurfaceFormatsKHR(m_surface);
  auto res = vk::check_surface_format_support(formats.value, default_surface_format, default_surface_color_space);
  yuri::info("format: {}, space_khr: {} 支持情况 -> {}", vk::to_string(default_surface_format), vk::to_string(default_surface_color_space), res);
}

} // namespace glfw