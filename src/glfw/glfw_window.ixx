export module glfw.window;

import glfw.api;
import glfw.context;
import vulkan;
import configuration;
import skia;
import yuri_log;
import std;
import ui;

using namespace ui::widgets;

export namespace glfw {

class Window : public Widget {
protected:
  GLFWwindow *m_window{}; // 窗口指针
  std::string m_title{};  // 窗口标题
  WindowContext context;  // context

public:
  HitTestGrid hit_test_grid;

  Window(int width, int height, std::string_view title = "yuri");
  ~Window() override;

  /**
   * 检查窗口状态
   * @return 是否应该关闭窗口
   */
  [[nodiscard]] bool shouldClose() const;

  /**
   * 展示窗口
   */
  void show();

  /**
   * 获取下一个渲染帧数据
   * 仅保证该帧在当前周期内有效，
   * 请不要保存该指针
   */
  render_frame* acquireNextFrame();

  /**
   * 开始执行绘制
   */
  void run();

  /**
   * 获取当前鼠标指针位置
   * @return 鼠标位置
   */
  skia::SkPoint getCursorPosition() const;

  /**
   * 展示debug信息
   */
  void showDebugInfo() const;

protected:
  /**
   * 窗口大小更改
   * @param width 新的宽度
   * @param height 新的高度
   */
  virtual void onResize(int width, int height);

  /**
   * 静态size更改回调函数
   */
  static void onResizeStatic(GLFWwindow* window, int width, int height);

  /**
   * 静态 鼠标移动事件回调
   */
  static void onMouseMoveStatic(GLFWwindow* window, double x, double y);

  /**
  * 静态 鼠标进入/进出事件回调
  */
  static void onMouseEnterStatic(GLFWwindow* window, int is_entered);

  /**
   * 静态 鼠标点击回调
   */
  static void onMouseButtonStatic(GLFWwindow* window, int button, int action, int mods);
};

Window::Window(const int width, const int height, const std::string_view title) :
  m_title(title),
  m_window(create_window(width, height, title)),
  context(m_window), hit_test_grid(width, height),
  Widget({0, 0, static_cast<float>(width), static_cast<float>(height)}, nullptr) {

  // GLFW 窗口大小回调
  glfwSetWindowUserPointer(m_window, this);
  glfwSetFramebufferSizeCallback(m_window, onResizeStatic);
  glfwSetCursorPosCallback(m_window, onMouseMoveStatic);
  glfwSetMouseButtonCallback(m_window, onMouseButtonStatic);
  glfwSetCursorEnterCallback(m_window, onMouseEnterStatic);

  // 打印debug信息
  if constexpr (is_debug_mode) {
    showDebugInfo();
  }
}

Window::~Window() {
  destroy_window(m_window);
}

void Window::onResize(int width, int height) {
  context.destroy_swapchain();
  context.create_swapchain();
  hit_test_grid = {width, height};
}

void Window::onResizeStatic(GLFWwindow *window, const int width, const int height) {
  // 忽略最小化
  if (width > 0 && height > 0) {
    const auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (width == self->width_ && height == self->height_) {
      return;
    }
    self->width_ = width;
    self->height_ = height;
    self->onResize(width, height);
  }
}

void Window::onMouseMoveStatic(GLFWwindow *window, const double x, const double y) {
  const auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
  self->onMouseMove(static_cast<float>(x), static_cast<float>(y));
}

void Window::onMouseEnterStatic(GLFWwindow *window, const int is_entered) {
  const auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (is_entered) {
    self->onMouseEnter();
  } else {
    self->onMouseLeave();
  }
}

void Window::onMouseButtonStatic(GLFWwindow *window,const  int button, const int action, const int mods) {
  const auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (button == left_mouse_button) {
    if (action == button_pressed) {
      self->onMouseLeftPressed();
    } else if (action == button_released) {
      self->onMouseLeftReleased();
    }
  }
}

bool Window::shouldClose() const {
  return glfwWindowShouldClose(m_window);
}

void Window::show() {
  glfwShowWindow(m_window);
  run();
}

render_frame* Window::acquireNextFrame() {
  return context.acquire_next_frame();
}

void Window::run() {
  while (!shouldClose()) {
    // 获取下一帧图像
    const auto frame = acquireNextFrame();
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

skia::SkPoint Window::getCursorPosition() const {
  double x, y;
  glfwGetCursorPos(m_window, &x, &y);
  return {
    static_cast<float>(x),
    static_cast<float>(y),
  };
}

void Window::showDebugInfo() const {
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