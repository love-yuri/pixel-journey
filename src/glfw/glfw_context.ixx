//
// Created by yuri on 2026/1/6.
//

export module glfw.context;

import std;
import vulkan.api;
import vulkan.context;
import vulkan_config;
import glfw.api;

/**
 * 每帧渲染数据
 */
export class render_frame {
public:

  vk::Semaphore render_finished;                  // 渲染完成的信号
  vk::Semaphore image_available;               // 图像获取完成的信号
  vk::Fence fence;                                // 帧渲染完成的栅栏
  std::vector<vk::CommandBuffer> command_buffers; // 专属命令缓冲区

  /**
   * 等待并重置fence
   */
  void wait_and_reset();
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
  std::vector<vk::Image> images;           // 所有图像
  std::vector<render_frame> frames;        // 当前帧渲染资源
};

// ReSharper disable once CppMemberFunctionMayBeConst
void render_frame::wait_and_reset() {
  const auto &device = vulkan_context->logic_device;
  vk::check_vk_result(
    device.waitForFences(fence, true, vk::wait_fence_timeout),
    "等待 Fence"
  );

  vk::check_vk_result(device.resetFences(fence),"重置 Fence");
}
