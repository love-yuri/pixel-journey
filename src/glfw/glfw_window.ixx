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

class glfw_window {
  int m_width;                // 窗口宽度
  int m_height;               // 窗口高度
  GLFWwindow *m_window{};     // 窗口指针
  std::string m_title{};      // 窗口标题
  window_context context;     // context

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
   * 检查窗口状态
   * @return 是否应该关闭窗口
   */
  bool should_close() const;

  /**
   * 展示窗口
   */
  void show() const;

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
  // virtual void render() = 0;

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
  // 忽略最小化
  if (width > 0 && height > 0) {
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

bool glfw_window::should_close() const {
  return glfwWindowShouldClose(m_window);
}

void glfw_window::show() const {
  glfwShowWindow(m_window);
}

render_frame* glfw_window::acquire_next_frame() {
  return context.acquire_next_frame();
}

void glfw_window::run() {
  while (!should_close()) {
    const auto frame = acquire_next_frame();
    frame->begin_frame();

    skia::GrVkImageInfo imageInfo{};
    imageInfo.fImage = *frame->image;
    imageInfo.fImageLayout = vk::VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.fFormat = vk::VK_FORMAT_B8G8R8A8_UNORM;
    imageInfo.fImageTiling = vk::VK_IMAGE_TILING_OPTIMAL;
    imageInfo.fLevelCount = 1;
    imageInfo.fCurrentQueueFamily = vulkan_context->queue_family_index;

    // skia::GrBackendRenderTarget backendRT = GrBackendRenderTargets::MakeVk(
    //   width(),
    //   height(),
    //   imageInfo
    // );

    // auto surface = SkSurfaces::WrapBackendRenderTarget(
    //   grContext.get(),
    //   backendRT,
    //   kTopLeft_GrSurfaceOrigin,
    //   kBGRA_8888_SkColorType,
    //   nullptr,
    //   &props
    // );
    //
    // if (surface == nullptr) {
    //   yuri::error("surface is null!");
    //   return 0;
    // }
    //
    // SkCanvas* canvas = surface->getCanvas();
    // canvas->clear(SK_ColorWHITE);
    // std::uniform_int_distribution dis_w(0, gw.width());
    // std::uniform_int_distribution dis_h(0, gw.height());
    //
    // SkPoint center = {
    //   static_cast<float>(gw.width() / 2),
    //   static_cast<float>(gw.height() / 2)
    // };
    //
    // canvas->drawCircle(center, 220, paint);
    // canvas->drawString(std::format("FPS: {:.1f}", fpsCounter.getFPS()).c_str(), 100, 830, font, paint);
    //
    // // Flush 并转换到呈现布局
    // grContext->flush(surface.get(), {}, &presentState);
    // grContext->submit(GrSyncCpu::kNo);

    frame->submit();
    frame->present();

    glfwPollEvents();
  }
}

void glfw_window::show_debug_info() const {
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