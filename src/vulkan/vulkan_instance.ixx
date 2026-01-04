export module vulkan:instance;

import yuri_log;
import vulkan_api;
import glfw_api;
import enum_utils;
import common_config;
import vulkan_config;
import std;

using namespace vk;

constexpr auto default_application_version = makeVersion(default_application_major, default_application_minor, default_application_patch);

/**
 * instance 创建信息
 * 大部分值使用默认值
 */
struct instance_create_info: InstanceCreateInfo {
  ApplicationInfo application_info {
    default_application_name,
    default_application_version,
    default_engin_name,
    default_application_version,
    vulkan_api_version
  };

  std::vector<const char*> extensions = glfw::get_vk_extensions();
  static constexpr const  char* layers[] = { vulkan_layer_khronos_validation };

  instance_create_info() {
    if constexpr (is_debug_mode) {
      enabledLayerCount = 1; // 启用的层数量
      extensions.push_back(vulkan_debug_extension_name);
    }

    yuri::info("GLFW 当前环境所需 extensions: {}", extensions.size());
    for (auto &instanceExtension : extensions) {
      yuri::info("extension {}", instanceExtension);
    }

    sType = StructureType::eInstanceCreateInfo,          // 结构体类型标识
    pApplicationInfo = &application_info,                // 指向应用程序信息
    ppEnabledLayerNames = layers,                        // 启用的层名称数组
    enabledExtensionCount = static_cast<std::uint32_t>(extensions.size()),  // 扩展数量
    ppEnabledExtensionNames = extensions.data(); // 启用扩展名称
  }
};

/**
 * vulkan_instance
 */
export class vulkan_instance: public Instance {
public:
  const instance_create_info info; // 创建instance 信息
  PhysicalDevice physical_device;  // 物理设备
  Device logic_device;             // 逻辑设备

// todo: 未完全替换c api前暂时public
public:
  vulkan_instance() {
    if (const auto res = createInstance(&info, nullptr, this); res != Result::eSuccess) {
      throw std::runtime_error(std::format(
        "创建vulkan_instance 失败: {}", enum_utils::name(res))
      );
    }

    auto [physical_device, index] = pick_physical_device_and_graphics_queue(*this);
    this->physical_device = physical_device;
    this->logic_device = create_logical_device(physical_device, index);

    const DebugUtilsMessengerCreateInfoEXT createInfo {
      {},
      DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | DebugUtilsMessageSeverityFlagBitsEXT::eWarning | DebugUtilsMessageSeverityFlagBitsEXT::eError,
      DebugUtilsMessageTypeFlagBitsEXT::eGeneral | DebugUtilsMessageTypeFlagBitsEXT::eValidation | DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
      reinterpret_cast<PFN_DebugUtilsMessengerCallbackEXT>(debugCallback),
      nullptr};

    const auto dispatcher = DispatchLoaderDynamic(*this, vkGetInstanceProcAddr);
    const auto res = createDebugUtilsMessengerEXT(createInfo, nullptr, dispatcher);
    check_vk_result(res.result, "Debug 附加消息");
  }
};