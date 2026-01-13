//
// Created by yuri on 2026/1/6.
//

export module glfw.context;

import std;
import vulkan;
import configuration;
import yuri_log;
import glfw.api;
import skia.api;

// 等待信息
constexpr vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eTransfer;

// 开始绘制信息
constexpr vk::CommandBufferBeginInfo command_buffer_begin_info{};

// 最大值
constexpr auto uint_32_max = std::numeric_limits<std::uint32_t>::max();

/**
 * 每帧渲染数据
 */
export class render_frame {
  vk::SwapchainKHR* swapchain = nullptr;        // 不持有交换链，仅作api使用
  vk::CommandBuffer* command_buffer = nullptr;  // 正在使用的command buffer

public:
  std::uint32_t image_index{};                       // image index
  vk::Image* image{};                                // 当前帧image
  vk::Semaphore* render_finished{};                  // 渲染完成的信号
  vk::Semaphore* image_available{};                  // 图像获取完成的信号
  vk::Fence* fence{};                                // 帧渲染完成的栅栏
  skia::SkSurface* sk_surface{};                     // surface
  std::vector<vk::CommandBuffer>* command_buffers{}; // 专属命令缓冲区

  explicit render_frame(vk::SwapchainKHR &swapchain);

  /**
   * 开始绘制
   * @return 需要录制的commandBuffer
   */
  vk::CommandBuffer* begin_frame();

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
export class window_context {
  int current_frame_index = -1;            // 当前使用的frame index
public:
  // 当前帧正在使用的frame
  std::unique_ptr<render_frame> current_frame{};
  glfw::GLFWwindow *window;                // GLFW窗口句柄
  vk::SurfaceKHR surface;                  // Vulkan Surface对象
  vk::Format format;                       // 交换链图像格式
  vk::ColorSpaceKHR color_space;           // 颜色空间标准
  vk::SurfaceCapabilitiesKHR capabilities; // Surface能力参数
  vk::SwapchainKHR swapchain;              // 交换链对象
  std::uint32_t image_count{};             // 图像数量: 默认min + 1
  std::vector<vk::Image> images;           // 所有图像
  std::vector<vk::Fence> fences;           // fence 同步量
  std::vector<vk::Semaphore> render_finished_semaphores;       // render 同步量
  std::vector<vk::Semaphore> image_available_semaphores;       // 图像可用同步量
  std::vector<std::vector<vk::CommandBuffer>> command_buffers; // buffers
  std::vector<skia::sk_sp<skia::SkSurface>> sk_surfaces;       // skia surface

  explicit window_context(glfw::GLFWwindow *window);

  /**
   * 析构函数，释放资源
   */
  ~window_context();

  /**
  * 创建交换链
  */
  void create_swapchain();

  /**
   * 销毁交换链
   */
  void destroy_swapchain() const;

  /**
   * 获取下一个渲染帧数据
   * 仅保证该帧在当前周期内有效，
   * 请不要保存该指针
   */
  render_frame* acquire_next_frame();
};

render_frame::render_frame(vk::SwapchainKHR &swapchain):swapchain(&swapchain) {

}

vk::CommandBuffer* render_frame::begin_frame() {
  command_buffer = &command_buffers->at(0);
  vk::check_vk_result(command_buffer->reset(), "重置cmd");
  vk::check_vk_result(command_buffer->begin(command_buffer_begin_info), "开始绘制");
  return command_buffer;
}

void render_frame::submit() const {
  vk::check_vk_result(command_buffer->end(), "结束录制");
  const vk::SubmitInfo submit_info {
    1, image_available,
    &wait_stage, 1,
    command_buffer, 1,
    render_finished
  };

  vk::check_vk_result(vulkan_context->queue.submit(submit_info, *fence), "提交绘制信息");
}

void render_frame::present() {
  const vk::PresentInfoKHR present_info {
    1,
    render_finished,
    1,
    swapchain,
    &image_index
  };

  vk::check_vk_result<false>(vulkan_context->queue.presentKHR(present_info), "呈现");
}

window_context::window_context(glfw::GLFWwindow *window) :
  window(window),
  format(vk::defaults::default_surface_format),
  color_space(vk::defaults::default_surface_color_space) {

  // 创建surface
  vk::VkSurfaceKHR surface{};
  glfw::glfwCreateWindowSurface(vulkan_context->instance, window, nullptr, &surface);
  this->surface = surface;

  // 创建交换链
  create_swapchain();
}

window_context::~window_context() {
  destroy_swapchain();
  vulkan_context->instance.destroySurfaceKHR(surface);
}

void window_context::create_swapchain() {
  // 获取caps
  capabilities = vulkan_context->get_surface_capabilities(surface);

  // 获取窗口size
  vk::Extent2D extent;
  if (capabilities.currentExtent.width != uint_32_max && capabilities.currentExtent.height != uint_32_max) {
    extent = capabilities.currentExtent;
  } else {
    extent = vk::get_buffer_size(window);
  }

  // 创建
  image_count = capabilities.minImageCount + 1;
  const vk::detail::swapchain_create_info info {
    surface,
    image_count,
    vk::defaults::default_surface_format,
    vk::defaults::default_surface_color_space,
    extent,
    capabilities.currentTransform
  };

  swapchain = vk::check_vk_result (
    vulkan_context->logic_device.createSwapchainKHR(info),
    "创建swapchain"
  );

  current_frame = std::make_unique<render_frame>(swapchain);
  images = vulkan_context->get_images(swapchain);

  // 创建渲染帧
  fences.resize(image_count);
  render_finished_semaphores.resize(image_count);
  image_available_semaphores.resize(image_count);
  command_buffers.resize(image_count);
  sk_surfaces.resize(image_count);

  for (auto i = 0; i < image_count; ++i) {
    render_finished_semaphores[i] = vulkan_context->create_semaphore();
    image_available_semaphores[i] = vulkan_context->create_semaphore();
    fences[i] = vulkan_context->create_fence();
    command_buffers[i] = vulkan_context->allocate_command_buffer();
    sk_surfaces[i] = skia::create_surface(
      &images[i],
      extent,
      vulkan_context->queue_family_index,
      vulkan_context->skia_direct_context.get()
    );

    // 再次提交更新layout
    vulkan_context->skia_direct_context->flush(sk_surfaces[i].get(), {}, &vulkan_context->present_state);
    vulkan_context->skia_direct_context->submit();
  }
}

void window_context::destroy_swapchain() const {
  auto _ = vulkan_context->logic_device.waitIdle();

  for (auto &k : command_buffers) {
    vulkan_context->logic_device.freeCommandBuffers(vulkan_context->command_pool, k);
  }

  for (const auto &k : image_available_semaphores) {
    vulkan_context->logic_device.destroySemaphore(k);
  }

  for (const auto &k : render_finished_semaphores) {
    vulkan_context->logic_device.destroySemaphore(k);
  }

  for (const auto &k : fences) {
    vulkan_context->logic_device.destroyFence(k);
  }

  vulkan_context->logic_device.destroySwapchainKHR(swapchain);

  // 释放显存
  vulkan_context->logic_device.freeMemory();
}

render_frame* window_context::acquire_next_frame() {
  // 获取下一帧
  current_frame_index = (current_frame_index + 1) % image_count;

  // 等待并重置fence
  const auto &device = vulkan_context->logic_device;
  vk::check_vk_result(
    device.waitForFences(fences[current_frame_index], true, vk::wait_fence_timeout),
    "等待 Fence"
  );
  vk::check_vk_result(device.resetFences(fences[current_frame_index]),"重置 Fence");

  // 拿到所需image_index
  std::uint32_t image_index;
  vk::check_vk_result(device.acquireNextImageKHR(
    swapchain, std::numeric_limits<std::uint64_t>::max(),
    image_available_semaphores[current_frame_index], nullptr, &image_index
  ), "acquire image index");

  // 更新当前帧数据
  current_frame->image_index = image_index;
  current_frame->image = &images[image_index];
  current_frame->render_finished = &render_finished_semaphores[image_index];
  current_frame->image_available = &image_available_semaphores[current_frame_index];
  current_frame->fence = &fences[current_frame_index];
  current_frame->command_buffers = &command_buffers[current_frame_index];
  current_frame->sk_surface = sk_surfaces[current_frame_index].get();

  return current_frame.get();
}
