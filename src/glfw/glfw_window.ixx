export module glfw.window;

import glfw.api;
import glfw.context;
import vulkan.detail;
import vulkan.api;
import vulkan.context;
import configuration;
import skia.api;
import yuri_log;
import std;

export namespace glfw {

class Window {
protected:
  int m_width;                // 窗口宽度
  int m_height;               // 窗口高度
  GLFWwindow *m_window{};     // 窗口指针
  std::string m_title{};      // 窗口标题
  WindowContext context;     // context

public:
  Window(int width, int height, std::string_view title = "yuri");
  virtual ~Window();

  /**
   * 获取窗口宽度
   */
  [[nodiscard]] int width() const;

  /**
   * 获取窗口高度
   */
  [[nodiscard]] int height() const;

  /**
   * 检查窗口状态
   * @return 是否应该关闭窗口
   */
  bool should_close() const;

  /**
   * 展示窗口
   */
  void show();

  /**
   * 获取下一个渲染帧数据
   * 仅保证该帧在当前周期内有效，
   * 请不要保存该指针
   */
  render_frame* acquire_next_frame();

  /**
   * 开始执行绘制
   */
  void run();

  /**
   * 绘制
   */
  virtual void render(skia::SkCanvas* canvas) = 0;

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

Window::Window(const int width, const int height, const std::string_view title) :
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

Window::~Window() {
  destroy_window(m_window);
}

void Window::on_resize(int width, int height) {
  context.destroy_swapchain();
  context.create_swapchain();
}

void Window::on_resize_static(GLFWwindow *window, const int width, const int height) {
  // 忽略最小化
  if (width > 0 && height > 0) {
    const auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (width == self->m_width && height == self->m_height) {
      return;
    }
    self->m_width = width;
    self->m_height = height;
    self->on_resize(width, height);
  }
}

int Window::width() const {
  return m_width;
}

int Window::height() const {
  return m_height;
}

bool Window::should_close() const {
  return glfwWindowShouldClose(m_window);
}

void Window::show() {
  glfwShowWindow(m_window);
  run();
}

render_frame* Window::acquire_next_frame() {
  return context.acquire_next_frame();
}

void Window::run() {
  while (!should_close()) {
    // 获取下一帧图像
    const auto frame = acquire_next_frame();
    frame->begin_frame();

    // 渲染
    render(frame->sk_surface->getCanvas());

    // 提交skia指令并转换布局
    vulkan_context->skia_direct_context->flush(frame->sk_surface, {}, &vulkan_context->present_state);
    vulkan_context->skia_direct_context->submit(skia::GrSyncCpu::kNo);

    // 提交绘制信息
    frame->submit();
    frame->present();

    glfwPollEvents();
  }
}

void Window::show_debug_info() const {
  auto caps = context.capabilities;
  yuri::info("Surface capabilities:");
  yuri::info("  minImageCount: {}", caps.minImageCount);
  yuri::info("  maxImageCount: {}", caps.maxImageCount);
  yuri::info("  currentExtent: {} x {}", caps.currentExtent.width, caps.currentExtent.height);

  const auto formats = vulkan_context->physical_device.getSurfaceFormatsKHR(context.surface);
  auto res = vk::check_surface_format_support(formats.value, vk::defaults::default_surface_format, vk::defaults::default_surface_color_space);
  yuri::info("format: {}, space_khr: {} 支持情况 -> {}", vk::to_string(vk::defaults::default_surface_format), vk::to_string(vk::defaults::default_surface_color_space), res);
}

} // namespace glfw