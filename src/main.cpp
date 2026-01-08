#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include "include/core/SkPaint.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/vk/GrVkTypes.h"
#include "include/core/SkFont.h"
#include "include/gpu/ganesh/vk/GrVkBackendSemaphore.h"
#include "include/gpu/vk/VulkanBackendContext.h"
#include "include/gpu/vk/VulkanExtensions.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/ganesh/vk/GrVkDirectContext.h"
#include "include/core/SkColorSpace.h"

#include <random>
#include <thread>
#include <magic_enum/magic_enum.hpp>

import yuri_log;
import std;
import vulkan;
import glfw;
import vulkan_config;

using namespace std::chrono_literals;

constexpr auto VK_LAYER_KHRONOS_validation = "VK_LAYER_KHRONOS_validation";
constexpr auto targe_format = VK_FORMAT_B8G8R8A8_UNORM;
constexpr auto target_color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

void create_vk_instance() {
  glfw::glfw_window gw = {
    200, 200, "yuri"};

  // 创建window和suffice
  auto window = gw.m_window;
  auto surface = gw.context.surface;

  auto physical_device = vulkan_context->physical_device;
  auto vk_device = vulkan_context->logic_device;

  // 创建graphic_queue
  auto graphicsQueue = vulkan_context->queue;

  // 创建swapchain
  auto swapchain = gw.context.swapchain;

  glfwShowWindow(window);
  std::uint32_t current_frame = 0;
  auto device = vulkan_context->logic_device;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution dis(0.0f, 1.0f);

  while (!glfwWindowShouldClose(window)) {
    constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 4;
    auto &frame = gw.context.frames[current_frame];

    // 1. 等待上一轮这个 Frame 完成
    frame.wait_and_reset();



    yuri::info("error: {}", current_frame);

    // 2. acquire image（用 Frame 自己的 semaphore）
    uint32_t image_index;
    auto result = device.acquireNextImageKHR (
      swapchain,
      UINT64_MAX,
      frame.image_available,
      nullptr,
      &image_index);

    gw.context.frames[image_index].wait_and_reset();

    yuri::info("select image: {}, frame: {}", image_index, current_frame);

    if (result == vk::Result::eErrorOutOfDateKHR) {
      // TODO: recreate swapchain
      continue;
    }

    vk::Image image = gw.context.images[image_index];
    vk::CommandBuffer cmd = frame.command_buffers[0];

    // 3. 录制命令
    cmd.reset();
    cmd.begin(vk::CommandBufferBeginInfo{});

    // UNDEFINED -> TRANSFER_DST
    cmd.pipelineBarrier(
      vk::PipelineStageFlagBits::eTopOfPipe,
      vk::PipelineStageFlagBits::eTransfer,
      {},
      nullptr,
      nullptr,
      vk::ImageMemoryBarrier{
        {},
        vk::AccessFlagBits::eTransferWrite,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        image,
        {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}});

    vk::ImageSubresourceRange range{
      vk::ImageAspectFlagBits::eColor,
      0,
      1,
      0,
      1};

    // 生成随机颜色
    vk::ClearColorValue clearColor{
      std::array{
        dis(gen),  // 红色分量
        dis(gen),  // 绿色分量
        dis(gen),  // 蓝色分量
        1.0f
      }
    };

    cmd.clearColorImage(
        image,
        vk::ImageLayout::eTransferDstOptimal,
        clearColor,
        range);

    // TRANSFER_DST -> PRESENT
    cmd.pipelineBarrier(
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eBottomOfPipe,
      {},
      nullptr,
      nullptr,
      vk::ImageMemoryBarrier{
        vk::AccessFlagBits::eTransferWrite,
        {},
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        image,
        {
          vk::ImageAspectFlagBits::eColor,
          0, 1, 0, 1}});

    cmd.end();

    // 4. submit
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eTransfer;

    graphicsQueue.submit(
      vk::SubmitInfo{
        1,
        &frame.image_available,
        &waitStage, 1,
        &cmd, 1,
        &frame.render_finished
      },
      frame.fence);

    graphicsQueue.presentKHR(
      vk::PresentInfoKHR{
        1,
        &frame.render_finished,
        1, &swapchain,
        &image_index
    });

    // 6. Frame 轮转
    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    // std::this_thread::sleep_for(100ms);
    yuri::info("render...");

    glfwPollEvents();
  }
}

int main() {
  create_vk_instance();
}