//
// Created by yuri on 2026/1/10.
//

export module vulkan.api:utils;

import :core;
import glfw.api;
import yuri_log;
import configuration;
import std;

export namespace vk {

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
VkBool32 debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT severity,
  VkDebugUtilsMessageTypeFlagsEXT type,
  const VkDebugUtilsMessengerCallbackDataEXT *data,
  void *userData) {
  yuri::error("[Vulkan] {}", data->pMessage);
  return false;
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
    for (std::uint32_t i = 0; i < queueFamilies.size(); ++i) {
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
  const std::vector deviceExtensions = { KHRSwapchainExtensionName };
  constexpr PhysicalDeviceFeatures features{};
  // features.samplerAnisotropy = VK_TRUE; // 需要再开

  DeviceCreateInfo createInfo;
  createInfo.queueCreateInfoCount = 1;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.enabledExtensionCount = static_cast<std::uint32_t>(deviceExtensions.size());
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

/**
 * 获取窗口的帧buffer大小
 * @return <width, height>
 */
Extent2D get_buffer_size(glfw::GLFWwindow* window) {
  int height, width;
  glfw::glfwGetFramebufferSize(window, &width, &height);
  return {
    static_cast<std::uint32_t>(width),
    static_cast<std::uint32_t>(height)
  };
}

}
