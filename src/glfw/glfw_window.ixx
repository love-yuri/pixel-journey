export module glfw.window;

import glfw.api;
import glfw.context;
import vulkan.detail;
import vulkan.api;
import vulkan.context;
import configuration;
import yuri_log;
import std;

export namespace glfw {

class glfw_window {
public:
  int m_width;                // 窗口宽度
  int m_height;               // 窗口高度
  GLFWwindow *m_window{};     // 窗口指针
  std::string m_title{};      // 窗口标题
  window_context context;   // context

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
   * 展示debug信息
   */
  void show_debug_info() const;

private:
  /**
   * 窗口大小更改
   * @param width 新的宽度
   * @param height 新的高度
   */
  void on_resize(int width, int height);

  /**
   * 静态size更改回调函数
   */
  static void on_resize_static(GLFWwindow* window, const int width, const int height);
};

glfw_window::glfw_window(const int width, const int height, const std::string_view title) :
  m_width(width),
  m_height(height),
  m_title(title),
  m_window(create_window(width, height, title)),
  context(m_window) {

  // GLFW 窗口大小回调
  glfwSetWindowUserPointer(m_window, this);
  glfwSetFramebufferSizeCallback(m_window, on_resize_static);

  // 打印debug信息
  if constexpr (is_debug_mode) {
    show_debug_info();
  }
}

glfw_window::~glfw_window() {
  destroy_window(m_window);
}

void glfw_window::on_resize(int width, int height) {
  context.destroy_swapchain();
  context.create_swapchain();
}

void glfw_window::on_resize_static(GLFWwindow *window, const int width, const int height) {
  if (width > 0 && height > 0) {  // 忽略最小化
    const auto self = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
    if (width == self->m_width && height == self->m_height) {
      return;
    }
    self->m_width = width;
    self->m_height = height;
    self->on_resize(width, height);
  }
}

int glfw_window::width() const {
  return m_width;
}

int glfw_window::height() const {
  return m_height;
}

void glfw_window::show_debug_info() const {
  auto caps = context.capabilities;
  yuri::info("Surface capabilities:");
  yuri::info("  minImageCount: {}", caps.minImageCount);
  yuri::info("  maxImageCount: {}", caps.maxImageCount);
  yuri::info("  currentExtent: {} x {}", caps.currentExtent.width, caps.currentExtent.height);

  const auto formats = vulkan_context->physical_device.getSurfaceFormatsKHR(context.surface);
  auto res = vk::check_surface_format_support(formats.value, vk::default_surface_format, vk::default_surface_color_space);
  yuri::info("format: {}, space_khr: {} 支持情况 -> {}", vk::to_string(vk::default_surface_format), vk::to_string(vk::default_surface_color_space), res);
}

} // namespace glfw