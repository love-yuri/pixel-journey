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
#include <magic_enum/magic_enum.hpp>

import yuri_log;
import std;
import vulkan;
import glfw;
import glfw.api;

constexpr auto VK_LAYER_KHRONOS_validation = "VK_LAYER_KHRONOS_validation";
constexpr auto targe_format = VK_FORMAT_B8G8R8A8_UNORM;
constexpr auto target_color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

VkInstance instance = nullptr;
GLFWwindow *window = nullptr;
VkSurfaceKHR surface = nullptr;
VkPhysicalDevice physical_device = nullptr;
VkDevice vk_device = nullptr;
VkQueue graphicsQueue = nullptr;
VkApplicationInfo application_info;
std::vector<const char *> instance_extensions;
VkSwapchainKHR swapchain = nullptr;

void check_res(const VkResult res, const std::string_view msg) {
  if (res != VK_SUCCESS) {
    throw std::runtime_error(std::format(
      "{} failed: {}", msg, magic_enum::enum_name(res))
    );
  }

  yuri::info("{} 成功!", msg);
}

void create_vk_swapchain() {
  // 创建khr
  VkSurfaceCapabilitiesKHR caps{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &caps);

  yuri::info("Surface capabilities:");
  yuri::info("  minImageCount: {}", caps.minImageCount);
  yuri::info("  maxImageCount: {}", caps.maxImageCount);
  yuri::info("  currentExtent: {} x {}", caps.currentExtent.width, caps.currentExtent.height);

  uint32_t imageCount = caps.minImageCount + 1;
  if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount) {
    imageCount = caps.maxImageCount;
  }

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  const VkSwapchainCreateInfoKHR swapchainInfo {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .surface = surface,
    .minImageCount = imageCount,
    .imageFormat = targe_format,
    .imageColorSpace = target_color_space,
    .imageExtent = {
      .width = static_cast<uint32_t>(width),
      .height = static_cast<uint32_t>(height),
    },
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .preTransform = caps.currentTransform,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = VK_PRESENT_MODE_FIFO_KHR,
    .clipped = VK_TRUE,
  };

  vkCreateSwapchainKHR(vk_device, &swapchainInfo, nullptr, &swapchain);
  if (swapchain == nullptr) {
    throw std::runtime_error("swapchain 创建失败!");
  }
}

void create_vk_instance() {

  glfw::glfw_window gw = {
    200, 200, "yuri"
  };

  // 创建window和suffice
  window = gw.m_window;
  surface = gw.m_surface;

  physical_device = vulkan_context->physical_device;
  vk_device = vulkan_context->logic_device;

  // 创建graphic_queue
  graphicsQueue = vulkan_context->queue;

  // 创建swapchain
  create_vk_swapchain();

  // 获取交换链图像数量
  uint32_t swapchainImageCount = 0;
  vkGetSwapchainImagesKHR(vk_device, swapchain, &swapchainImageCount, nullptr);
  std::vector<VkImage> swapchainImages(swapchainImageCount);
  vkGetSwapchainImagesKHR(vk_device, swapchain, &swapchainImageCount, swapchainImages.data());
  yuri::info("Swapchain images: {}", swapchainImageCount);

  VkCommandPoolCreateInfo poolInfo {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = 0,
  };

  VkCommandPool commandPool = nullptr;
  vkCreateCommandPool(vk_device, &poolInfo, nullptr, &commandPool);

  VkCommandBufferAllocateInfo allocInfo {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = commandPool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  VkCommandBuffer cmd = VK_NULL_HANDLE;
  vkAllocateCommandBuffers(vk_device, &allocInfo, &cmd);

  VkFenceCreateInfo fenceInfo{
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };

  VkFence fence;
  vkCreateFence(vk_device, &fenceInfo, nullptr, &fence);

  VkSemaphoreCreateInfo semInfo{
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };

  VkSemaphore imageAvailable;
  VkSemaphore renderFinished;

  vkCreateSemaphore(vk_device, &semInfo, nullptr, &imageAvailable);
  vkCreateSemaphore(vk_device, &semInfo, nullptr, &renderFinished);

  uint32_t imageIndex;
  vkAcquireNextImageKHR(vk_device, swapchain, UINT64_MAX, imageAvailable, nullptr, &imageIndex);

  vkResetCommandBuffer(cmd, 0);

  VkCommandBufferBeginInfo beginInfo{
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
  };

  vkBeginCommandBuffer(cmd, &beginInfo);

  VkImageMemoryBarrier barrier1 {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = swapchainImages[imageIndex],
    .subresourceRange = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
  };

  vkCmdPipelineBarrier(
    cmd,
    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
    0,
    0,
    nullptr,
    0,
    nullptr,
    1,
    &barrier1);

  VkClearColorValue color{{0.2f, 0.2f, 0.2f, 1.0f}};
  VkImageSubresourceRange range{
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .baseMipLevel = 0,
    .levelCount = 1,
    .baseArrayLayer = 0,
    .layerCount = 1,
  };

  vkCmdClearColorImage(
    cmd,
    swapchainImages[imageIndex],
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    &color,
    1,
    &range);

  VkImageMemoryBarrier barrier2{
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = swapchainImages[imageIndex],
    .subresourceRange = barrier1.subresourceRange,
  };

  vkCmdPipelineBarrier(
    cmd,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    0,
    0,
    nullptr,
    0,
    nullptr,
    1,
    &barrier2);

  vkEndCommandBuffer(cmd);

  VkPipelineStageFlags waitStage =
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  VkSubmitInfo submitInfo{
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = &imageAvailable,
    .pWaitDstStageMask = &waitStage,
    .commandBufferCount = 1,
    .pCommandBuffers = &cmd,
    .signalSemaphoreCount = 1,
    .pSignalSemaphores = &renderFinished,
  };

  vkResetFences(vk_device, 1, &fence);
  vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence);

  VkPresentInfoKHR presentInfo {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = &renderFinished,
    .swapchainCount = 1,
    .pSwapchains = &swapchain,
    .pImageIndices = &imageIndex,
  };

  vkQueuePresentKHR(graphicsQueue, &presentInfo);
  vkWaitForFences(vk_device, 1, &fence, VK_TRUE, UINT64_MAX);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

int main() {


  create_vk_instance();
}