module;
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_SMART_HANDLE
#include <vulkan/vulkan.hpp>
export module vulkan.api;

import yuri_log;
import glfw.api;
import configuration;

/**
 * detail
 */
export namespace vk::detail {
  using detail::getDispatchLoaderStatic;
}

export namespace vk {

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

/* constexpr */
constexpr auto vulkan_api_version = VK_API_VERSION_1_4;  // 使用的vulkan版本
constexpr auto vulkan_debug_extension_name = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
constexpr auto vulkan_layer_khronos_validation = "VK_LAYER_KHRONOS_validation";

/**
 * 获取版本字符串
 * @return 返回xx.xx.xx格式
 */
std::string version_string(const std::uint32_t version) {
  return std::format("{}.{}.{}",
     versionMajor(version),
     versionMinor(version),
     versionPatch(version)
  );
}

/**
 * debug callback
 * @return
 */
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT severity,
  VkDebugUtilsMessageTypeFlagsEXT type,
  const VkDebugUtilsMessengerCallbackDataEXT *data,
  void *userData) {
  yuri::error("[Vulkan] {}", data->pMessage);
  return VK_FALSE;
}

/**
 * 检查vulkan操作信息是否正确，并可以设置是否抛出异常
 * @tparam ThrowOnError 是否抛出异常
 * @param result 返回值
 * @param msg 操作信息
 */
template <bool ThrowOnError = true>
bool check_vk_result(const Result result, std::string_view msg = "vulkan操作") {
  if (result != Result::eSuccess) {
    const auto error_msg = std::format("{} 失败! 错误码: {}", msg, to_string(result));
    if constexpr (ThrowOnError) {
      throw std::runtime_error(error_msg);
    } else {
      yuri::error(error_msg);
    }
    return false;
  }

  return true;
}

/**
 * 检查vulkan操作信息是否正确，并可以设置是否抛出异常
 * @param res 操作结果
 * @param msg 操作信息
 */
template <typename T>
T check_vk_result(const ResultValue<T>& res, const std::string_view msg = "vulkan操作") {
  if (res.result != Result::eSuccess) {
    const auto error_msg = std::format("{} 失败! 错误码: {}", msg, to_string(res.result));
    throw std::runtime_error(error_msg);
  }

  return res.value;
}

/**
 * 获取物理设备和queue的index，默认返回第一个可用
 * debug
 */
std::tuple<PhysicalDevice, std::uint32_t> pick_physical_device_and_graphics_queue(const Instance& instance) {
  const auto devices = instance.enumeratePhysicalDevices();
  check_vk_result(devices.result, "获取Graphics");
  yuri::info("找到 {} 个GPU设备", devices.value.size());

  // 遍历组内的每个物理设备
  for (const auto device: devices.value) {
    if constexpr (is_debug_mode) {
      // 获取设备属性
      PhysicalDeviceProperties props = device.getProperties();

      // 打印设备详细信息
      yuri::info("GPU {}", props.deviceName.data());
      yuri::info("  - API 版本: {}", version_string(props.apiVersion));
      yuri::info("  - 供应商ID: 0x{:X}", props.vendorID);
      yuri::info("  - 设备ID: 0x{:X}", props.deviceID);
      yuri::info("  - 类型: {}", to_string(props.deviceType));
    }

    const auto queueFamilies = device.getQueueFamilyProperties();
    for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
      if (queueFamilies[i].queueFlags & QueueFlagBits::eGraphics) {
        return { device, i };
      }
    }
  }

  throw std::runtime_error("没有发现可用GPU设备!");
}

/**
 * 自动创建逻辑设备
 * @param physical_device 物理设备
 * @param index 需要创建的队列号
 * @return 返回创建设备
 */
Device create_logical_device(const PhysicalDevice& physical_device, const std::uint32_t index) {
  constexpr float queuePriority = 1.0f;
  const DeviceQueueCreateInfo queueCreateInfo { {}, index, 1, &queuePriority };

  // 必须启用的设备扩展（窗口系统）
  const std::vector deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
  constexpr PhysicalDeviceFeatures features{};
  // features.samplerAnisotropy = VK_TRUE; // 需要再开

  DeviceCreateInfo createInfo;
  createInfo.queueCreateInfoCount = 1;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();
  createInfo.pEnabledFeatures = &features;

  const auto res = physical_device.createDevice(createInfo);
  check_vk_result(res.result, "创建逻辑设备");
  return res.value;
}

/**
 * 检查是否支持所需format
 * @param format_khrs format合集
 * @param format 目标 format
 * @param space_khr 目标 space khr
 * @return 是否支持
 */
bool check_surface_format_support(const std::vector<SurfaceFormatKHR>& format_khrs, Format format, ColorSpaceKHR space_khr) {
  return std::ranges::any_of(format_khrs, [format, space_khr](const SurfaceFormatKHR& format_khr) {
    return format_khr.format == format && format_khr.colorSpace == space_khr;
  });
}

}