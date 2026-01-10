//
// Created by yuri on 2026/1/10.
//

export module vulkan.api:config;

import :core;

export namespace vk {

/* constexpr */
constexpr auto vulkan_api_version = makeVersion(1, 4, 0);  // 使用的vulkan版本
constexpr auto vulkan_debug_extension_name = EXTDebugUtilsExtensionName;
constexpr auto vulkan_layer_khronos_validation = "VK_LAYER_KHRONOS_validation";

/**
  * 默认使用以下两个，大部分设备都支持
  * 默认不会进行检查
  */
constexpr auto default_surface_format = Format::eB8G8R8A8Unorm;             // 默认format
constexpr auto default_surface_color_space = ColorSpaceKHR::eSrgbNonlinear; // 默认color_space

}