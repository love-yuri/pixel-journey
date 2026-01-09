//
// Created by yuri on 2026/1/6.
//

export module glfw.context;

import std;
import vulkan.api;
import vulkan.context;
import configuration;
import yuri_log;
import glfw.api;

/**
 * 每帧渲染数据
 */
export class render_frame {
public:
  std::uint32_t image_index{};                       // image index
  vk::Image* image{};                                // 当前帧image
  vk::Semaphore* render_finished{};                  // 渲染完成的信号
  vk::Semaphore* image_available{};                  // 图像获取完成的信号
  vk::Fence* fence{};                                // 帧渲染完成的栅栏
  std::vector<vk::CommandBuffer>* command_buffers{}; // 专属命令缓冲区
};

/**
 * GLFW窗口与Vulkan渲染上下文整合结构体
 * 管理窗口、Surface、交换链及帧渲染资源
 */
export class glfw_context {
public:
  glfw::GLFWwindow *window;                // GLFW窗口句柄
  vk::SurfaceKHR surface;                  // Vulkan Surface对象
  vk::Format format;                       // 交换链图像格式
  vk::ColorSpaceKHR colorSpace;            // 颜色空间标准
  vk::SurfaceCapabilitiesKHR capabilities; // Surface能力参数
  vk::SwapchainKHR swapchain;              // 交换链对象
  std::uint32_t image_count;               // 图像数量: 默认min + 1
  int current_frame = -1;                  // 当前使用的frame
  std::vector<vk::Image> images;           // 所有图像
  std::vector<vk::Fence> fences;           // fence 同步量
  std::vector<vk::Semaphore> render_finished_semaphores;       // render 同步量
  std::vector<vk::Semaphore> image_available_semaphores;       // 图像可用同步量
  std::vector<std::vector<vk::CommandBuffer>> command_buffers; // buffers

  /**
   * 获取下一个渲染帧数据
   * @return 下一帧数据
   */
  render_frame acquire_next_frame();
};

render_frame glfw_context::acquire_next_frame() {
  // 获取下一帧
  current_frame = (current_frame + 1) % image_count;

  // 等待并重置fence
  const auto &device = vulkan_context->logic_device;
  vk::check_vk_result(
    device.waitForFences(fences[current_frame], true, vk::wait_fence_timeout),
    "等待 Fence"
  );
  vk::check_vk_result(device.resetFences(fences[current_frame]),"重置 Fence");

  // 拿到所需image_index
  std::uint32_t image_index;
  vk::check_vk_result(device.acquireNextImageKHR(
    swapchain, std::numeric_limits<std::uint64_t>::max(),
    image_available_semaphores[current_frame], nullptr, &image_index
  ), "acquire image index");

  // 返回frame 帧
  return {
    image_index,
    &images[image_index],
    &render_finished_semaphores[image_index],
    &image_available_semaphores[current_frame],
    &fences[current_frame],
    &command_buffers[current_frame],
  };
}
