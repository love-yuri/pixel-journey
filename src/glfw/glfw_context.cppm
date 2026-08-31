//
// Created by yuri on 2026/1/6.
//

export module yuri.glfw.context;

import std;
import yuri.vulkan;
import yuri.configuration;
import yuri.log;
import yuri.glfw.api;
import yuri.skia.api;

// 最大值
constexpr auto uint_32_max = std::numeric_limits<std::uint32_t>::max();

/**
 * 每帧渲染数据
 */
export class render_frame {
  vk::SwapchainKHR *swapchain = nullptr; // 不持有交换链，仅作api使用

public:
  std::uint32_t image_index{};      // image index
  vk::Image *image{};               // 当前帧image
  vk::Semaphore *render_finished{}; // 渲染完成的信号
  vk::Semaphore *image_available{}; // 图像获取完成的信号
  vk::Fence *frame_fence{};         // 帧槽位完成栅栏
  skia::SkSurface *sk_surface{};    // surface

  explicit render_frame(vk::SwapchainKHR &swapchain);

  /**
   * 提交渲染指令
   */
  void submit() const;

  /**
   * 展示渲染内容
   */
  void present();
};

/**
 * GLFW窗口与Vulkan渲染上下文整合结构体
 * 管理窗口、Surface、交换链及帧渲染资源
 */
export class WindowContext {
  int current_frame_index = -1; // 当前使用的frame index
public:
  // 当前帧正在使用的frame
  std::unique_ptr<render_frame> current_frame{};
  glfw::GLFWwindow *window;                              // GLFW窗口句柄
  vk::SurfaceKHR surface;                                // Vulkan Surface对象
  vk::Format format;                                     // 交换链图像格式
  vk::ColorSpaceKHR color_space;                         // 颜色空间标准
  vk::SurfaceCapabilitiesKHR capabilities;               // Surface能力参数
  vk::SwapchainKHR swapchain;                            // 交换链对象
  std::uint32_t image_count{};                           // 图像数量: 默认min + 1
  std::vector<vk::Image> images;                         // 所有图像
  std::vector<vk::Semaphore> render_finished_semaphores; // render 同步量
  std::vector<vk::Semaphore> image_available_semaphores; // 图像可用同步量
  std::vector<vk::Fence> frame_fences;                   // 帧槽位完成栅栏
  std::vector<skia::sk_sp<skia::SkSurface>> sk_surfaces; // skia surface

  explicit WindowContext(glfw::GLFWwindow *window);

  /**
   * 析构函数，释放资源
   */
  ~WindowContext();

  /**
   * 创建交换链
   */
  void create_swapchain();

  /**
   * 销毁交换链
   */
  void destroy_swapchain();

  /**
   * 获取下一个渲染帧数据
   * 仅保证该帧在当前周期内有效，
   * 请不要保存该指针
   */
  render_frame *acquire_next_frame();
};

render_frame::render_frame(vk::SwapchainKHR &swapchain) : swapchain(&swapchain) {
}

void render_frame::submit() const {
  const auto wait_semaphore = skia::GrBackendSemaphores::MakeVk(*image_available);
  vulkan_context->skia_direct_context->wait(1, &wait_semaphore, false);

  auto signal_semaphore = skia::GrBackendSemaphores::MakeVk(*render_finished);
  const skia::GrFlushInfo flush_info{ 1, {}, &signal_semaphore };

  vulkan_context->skia_direct_context->flush(
    sk_surface, flush_info, &vulkan_context->present_state
  );
  vulkan_context->skia_direct_context->submit(skia::GrSyncCpu::kNo);
  vk::check_vk_result(vulkan_context->queue.submit(0, nullptr, *frame_fence), "提交帧完成 Fence");
}

void render_frame::present() {
  const vk::PresentInfoKHR present_info{ 1, render_finished, 1, swapchain, &image_index };

  vk::check_vk_result<false>(vulkan_context->queue.presentKHR(present_info), "呈现");
}

WindowContext::WindowContext(glfw::GLFWwindow *window) :
  window(window), format(vk::defaults::default_surface_format),
  color_space(vk::defaults::default_surface_color_space) {
  // 创建surface
  vk::VkSurfaceKHR surface{};
  glfw::glfwCreateWindowSurface(vulkan_context->instance, window, nullptr, &surface);
  this->surface = surface;

  // 创建交换链
  create_swapchain();
}

WindowContext::~WindowContext() {
  destroy_swapchain();
  vulkan_context->instance.destroySurfaceKHR(surface);
}

void WindowContext::create_swapchain() {
  // 获取caps
  capabilities = vulkan_context->get_surface_capabilities(surface);

  // 获取窗口size
  vk::Extent2D extent;
  if (capabilities.currentExtent.width != uint_32_max
      && capabilities.currentExtent.height != uint_32_max) {
    extent = capabilities.currentExtent;
  } else {
    extent = vk::get_buffer_size(window);
  }

  const auto surface_formats = vk::check_vk_result(
    vulkan_context->physical_device.getSurfaceFormatsKHR(surface), "获取 Surface Formats"
  );
  const auto selected_format = vk::choose_surface_format(surface_formats);
  format = selected_format.format;
  color_space = selected_format.colorSpace;

  const auto requested_image_count = capabilities.minImageCount + 1;
  auto selected_image_count = requested_image_count;
  if (capabilities.maxImageCount > 0) {
    selected_image_count = std::min(selected_image_count, capabilities.maxImageCount);
  }

  if constexpr (is_debug_mode) {
    yuri::info("Surface capabilities:");
    yuri::info("  minImageCount: {}", capabilities.minImageCount);
    yuri::info("  maxImageCount: {}", capabilities.maxImageCount);
    yuri::info(
      "  currentExtent: {} x {}",
      capabilities.currentExtent.width,
      capabilities.currentExtent.height
    );
    yuri::info("  selectedExtent: {} x {}", extent.width, extent.height);
    yuri::info("Swapchain image count:");
    yuri::info("  requested: {}", requested_image_count);
    yuri::info("  selectedForCreate: {}", selected_image_count);
    yuri::info(
      "Swapchain format: format={}, colorSpace={}",
      vk::to_string(format),
      vk::to_string(color_space)
    );
  }

  // 创建
  image_count = selected_image_count;
  const vk::detail::swapchain_create_info info{ surface, image_count,
                                                format,  color_space,
                                                extent,  capabilities.currentTransform };

  swapchain =
    vk::check_vk_result(vulkan_context->logic_device.createSwapchainKHR(info), "创建swapchain");

  current_frame = std::make_unique<render_frame>(swapchain);
  images = vulkan_context->get_images(swapchain);
  if (images.empty()) {
    throw std::runtime_error("获取到的 swapchain image 数量为 0");
  }
  image_count = static_cast<std::uint32_t>(images.size());
  yuri::info("  actualImages: {}", image_count);

  // 创建渲染帧
  render_finished_semaphores.resize(image_count);
  image_available_semaphores.resize(image_count);
  frame_fences.resize(image_count);
  sk_surfaces.resize(image_count);

  for (std::uint32_t i = 0; i < image_count; ++i) {
    render_finished_semaphores[i] = vulkan_context->create_semaphore();
    image_available_semaphores[i] = vulkan_context->create_semaphore();
    frame_fences[i] = vulkan_context->create_fence();
    sk_surfaces[i] = skia::create_surface(
      &images[i],
      extent,
      format,
      vulkan_context->queue_family_index,
      vulkan_context->skia_direct_context.get()
    );
  }
}

void WindowContext::destroy_swapchain() {
  if (!swapchain) return;

  vulkan_context->skia_direct_context->flushAndSubmit(skia::GrSyncCpu::kYes);
  void(vulkan_context->logic_device.waitIdle());

  sk_surfaces.clear();
  images.clear();

  for (const auto &k : frame_fences) {
    vulkan_context->logic_device.destroyFence(k);
  }
  frame_fences.clear();

  for (const auto &k : image_available_semaphores) {
    vulkan_context->logic_device.destroySemaphore(k);
  }
  image_available_semaphores.clear();

  for (const auto &k : render_finished_semaphores) {
    vulkan_context->logic_device.destroySemaphore(k);
  }
  render_finished_semaphores.clear();

  vulkan_context->logic_device.destroySwapchainKHR(swapchain);
  swapchain = nullptr;
  current_frame.reset();
  current_frame_index = -1;
  image_count = 0;

  // 释放显存
  vulkan_context->logic_device.freeMemory();
}

render_frame *WindowContext::acquire_next_frame() {
  // 获取下一帧
  current_frame_index = (current_frame_index + 1) % image_count;

  // 等待当前帧槽位释放
  const auto &frame_fence = frame_fences[current_frame_index];
  vk::check_vk_result(
    vulkan_context->logic_device.waitForFences(
      frame_fence, true, std::numeric_limits<std::uint64_t>::max()
    ),
    "等待帧完成 Fence"
  );
  vk::check_vk_result(vulkan_context->logic_device.resetFences(frame_fence), "重置帧完成 Fence");

  // 拿到所需image_index
  std::uint32_t image_index;
  vk::check_vk_result(
    vulkan_context->logic_device.acquireNextImageKHR(
      swapchain,
      std::numeric_limits<std::uint64_t>::max(),
      image_available_semaphores[current_frame_index],
      nullptr,
      &image_index
    ),
    "acquire image index"
  );

  // 更新当前帧数据
  current_frame->image_index = image_index;
  current_frame->image = &images[image_index];
  current_frame->render_finished = &render_finished_semaphores[image_index];
  current_frame->image_available = &image_available_semaphores[current_frame_index];
  current_frame->frame_fence = &frame_fences[current_frame_index];
  current_frame->sk_surface = sk_surfaces[image_index].get();

  return current_frame.get();
}
