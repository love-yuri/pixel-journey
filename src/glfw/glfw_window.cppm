// ReSharper disable CppTooWideScopeInitStatement
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
  std::string title_{};          // 窗口标题
  GLFWwindow *window_ = nullptr; // 窗口指针
  WindowContext context_;        // context
  float cursor_x = 0.0f;         // 鼠标位置—x
  float cursor_y = 0.0f;         // 鼠标位置-y

public:
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
  render_frame *acquireNextFrame();

  /**
   * 开始执行绘制
   */
  void run();

  /**
   * 获取当前鼠标指针位置
   * @return 鼠标位置
   */
  [[nodiscard]] skia::SkPoint getCursorPosition() const;

  /**
   * 展示debug信息
   */
  void showDebugInfo() const;

protected:
  void layoutChildren() override;

  friend void onResizeStatic(GLFWwindow *window, int width, int height);
  friend void onMouseMoveStatic(GLFWwindow *window, double x, double y);
  friend void onMouseEnterStatic(GLFWwindow *window, int is_entered);
  friend void onMouseButtonStatic(GLFWwindow *window, int button, int action, int mods);
  friend void onSetWindowIconifyStatic(GLFWwindow *window, int iconified);
};

/**
 * 静态size更改回调函数
 */
void onResizeStatic(GLFWwindow *window, const int width, const int height) {
  // 忽略最小化
  if (width > 0 && height > 0) {
    const auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (width == static_cast<int>(self->width_) && height == static_cast<int>(self->height_)) {
      return;
    }
    // 重建交换链
    self->context_.destroy_swapchain();
    self->context_.create_swapchain();

    // 调用resize
    self->resize(static_cast<float>(width), static_cast<float>(height));
  }
}

/**
 * 静态 鼠标移动事件回调
 */
void onMouseMoveStatic(GLFWwindow *window, const double x, const double y) {
  const auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
  if (self->visible) {
    self->cursor_x = static_cast<float>(x);
    self->cursor_y = static_cast<float>(y);
    self->MouseMove(self->cursor_x, self->cursor_y);
  }
}

/**
 * 静态 鼠标进入/进出事件回调
 */
void onMouseEnterStatic(GLFWwindow *window, const int is_entered) {
  const auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
  if (self->visible && !is_entered) {
    self->onMouseLeave(self->cursor_x, self->cursor_y);
  }
}

/**
 * 静态 鼠标点击回调
 */
void onMouseButtonStatic(GLFWwindow *window, const int button, const int action, const int mods) {
  const auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
  if (self->visible && button == left_mouse_button) {
    if (action == button_pressed) {
      self->MouseLeftPressed(self->cursor_x, self->cursor_y);
    } else if (action == button_released) {
      self->MouseLeftReleased(self->cursor_x, self->cursor_y);
    }
  }
}

void onSetWindowIconifyStatic(GLFWwindow *window, const int iconified) {
  const auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
  self->visible = !iconified;
}

Window::Window(const int width, const int height, const std::string_view title) :
  Widget(nullptr),
  title_(title),
  window_(create_window(width, height, title_)),
  context_(window_) {

  // 更新几何信息
  setGeometry(0, 0, width, height); // NOLINT(*-narrowing-conversions)

  // 窗口居中
  auto [w, h] = getPrimaryMonitorSize();
  glfwSetWindowPos(window_, (w - width_) / 2, (h - height_) / 2);  // NOLINT(*-narrowing-conversions)

  // GLFW 窗口大小回调
  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, onResizeStatic);
  glfwSetCursorPosCallback(window_, onMouseMoveStatic);
  glfwSetMouseButtonCallback(window_, onMouseButtonStatic);
  glfwSetCursorEnterCallback(window_, onMouseEnterStatic);
  glfwSetWindowIconifyCallback(window_, onSetWindowIconifyStatic);

  // 打印debug信息
  if constexpr (is_debug_mode) {
    showDebugInfo();
  }
}

Window::~Window() {
  destroy_window(window_);
}

void Window::layoutChildren() {
  // 重新布局
  if (layout_ != nullptr) {
    layout_->apply(this);
  }

  // 更新自控件布局
  for (const auto child : this->children_) {
    child->updateLayout();
  }
}

bool Window::shouldClose() const {
  return glfwWindowShouldClose(window_);
}

void Window::show() {
  glfwShowWindow(window_);
  run();
}

render_frame *Window::acquireNextFrame() {
  return context_.acquire_next_frame();
}

void Window::run() {
  while (!shouldClose()) {
    if (!visible) {
      // 不可见时阻塞等待事件
      glfwWaitEvents();
      continue;
    }

    // 获取下一帧图像
    const auto frame = acquireNextFrame();
    frame->begin_frame();

    // 更新动画
    animation_manager->update();

    // 更新布局
    updateLayout();

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
  glfwGetCursorPos(window_, &x, &y);
  return {
    static_cast<float>(x),
    static_cast<float>(y),
  };
}

void Window::showDebugInfo() const { // NOLINT(*-convert-member-functions-to-static)
  auto caps = context_.capabilities;
  yuri::info("Surface capabilities:");
  yuri::info("  minImageCount: {}", caps.minImageCount);
  yuri::info("  maxImageCount: {}", caps.maxImageCount);
  yuri::info("  currentExtent: {} x {}", caps.currentExtent.width, caps.currentExtent.height);

  const auto formats = vulkan_context->physical_device.getSurfaceFormatsKHR(context_.surface);
  auto res = vk::check_surface_format_support(formats.value, vk::defaults::default_surface_format, vk::defaults::default_surface_color_space);
  yuri::info("format: {}, space_khr: {} 支持情况 -> {}", vk::to_string(vk::defaults::default_surface_format), vk::to_string(vk::defaults::default_surface_color_space), res);
}

} // namespace glfw