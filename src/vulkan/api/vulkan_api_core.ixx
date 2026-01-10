module;
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
export module vulkan.api:core;

import std;

/**
 * detail
 */
export namespace vk::detail {

using detail::getDispatchLoaderStatic;
using detail::DispatchLoaderDynamic;

} // namespace vk::detail

export namespace vk {

using ::VkBool32;
using vk::ResultValue;
using ::VkDebugUtilsMessengerCallbackDataEXT;
using ::VkDebugUtilsMessageTypeFlagsEXT;
using ::VkDebugUtilsMessageSeverityFlagBitsEXT;
using vk::QueueFlagBits;
using vk::KHRSwapchainExtensionName;
using vk::EXTDebugUtilsExtensionName;
using vk::DeviceQueueCreateInfo;
using vk::DeviceCreateInfo;
using vk::SurfaceFormatKHR;
using ::vkDestroyDebugUtilsMessengerEXT;
using vk::versionMajor;
using vk::versionMinor;
using vk::versionPatch;
using vk::makeVersion;
using vk::to_string;
using vk::Extent2D;
using vk::Instance;
using vk::InstanceCreateInfo;
using vk::ApplicationInfo;
using vk::StructureType;
using vk::SharingMode;
using vk::DebugUtilsMessengerCreateInfoEXT;
using vk::DebugUtilsMessengerCreateFlagsEXT;
using vk::DebugUtilsMessageSeverityFlagBitsEXT;
using vk::DebugUtilsMessageTypeFlagBitsEXT;
using vk::DebugUtilsMessageSeverityFlagsEXT;
using vk::PFN_DebugUtilsMessengerCallbackEXT;
using vk::DebugUtilsMessengerEXT;
using ::vkGetInstanceProcAddr;
using detail::DispatchLoaderDynamic;
using vk::operator|;
using ::VkInstance;
using vk::createInstance;
using vk::Result;
using vk::PhysicalDevice;
using vk::Device;
using vk::PhysicalDeviceProperties;
using vk::PhysicalDeviceFeatures;
using vk::Queue;
using vk::SurfaceKHR;
using ::VkSurfaceKHR;
using vk::Format;
using vk::ScopeKHR;
using vk::ColorSpaceKHR;
using vk::SurfaceCapabilitiesKHR;
using vk::SwapchainCreateInfoKHR;
using vk::SwapchainKHR;
using vk::ImageUsageFlagBits;
using vk::PresentModeKHR;
using vk::CompositeAlphaFlagBitsKHR;
using vk::SurfaceTransformFlagBitsKHR;
using vk::Image;
using vk::ImageView;
using vk::CommandBuffer;
using vk::CommandPool;
using vk::CommandPoolCreateInfo;
using vk::CommandPoolCreateFlags;
using vk::CommandPoolCreateFlagBits;
using vk::CommandBufferAllocateInfo;
using vk::CommandBufferLevel;
using vk::FenceCreateInfo;
using vk::Fence;
using vk::FenceCreateFlagBits;
using vk::Semaphore;
using vk::SemaphoreCreateInfo;
using vk::CommandBufferBeginInfo;
using vk::ImageMemoryBarrier;
using vk::AccessFlagBits;
using vk::AccessFlags;
using vk::ImageLayout;
using vk::ImageSubresourceRange;
using vk::ImageAspectFlagBits;
using vk::PipelineStageFlagBits;
using vk::DependencyFlags;
using vk::ClearColorValue;
using vk::PipelineStageFlags;
using vk::SubmitInfo;
using vk::PresentInfoKHR;
using vk::QueueFamilyIgnored;

} // namespace vk