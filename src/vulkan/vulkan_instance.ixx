export module vulkan:instance;

import yuri_log;
import vulkan_api;
import glfw_api;
import common_config;
import vulkan_config;
import std;

using namespace vk;

constexpr auto default_application_version = makeVersion(default_application_major, default_application_minor, default_application_patch);

/**
 * @brief Vulkan Instance 创建信息封装
 *
 * instance_create_info 继承自 vk::InstanceCreateInfo，
 * 用于简化 Vulkan Instance 创建过程中的参数配置。
 *
 * 设计目标：
 * - 大部分参数使用推荐或默认值
 * - 自动从 GLFW 获取所需 Instance extensions
 * - 在 Debug 模式下自动启用 validation layer 和 debug 扩展
 *
 * 适用于：
 * - 基于 GLFW 的窗口系统
 * - 单 Instance 的常规 Vulkan 应用
 */
struct instance_create_info : InstanceCreateInfo {

  /**
   * @brief Vulkan 应用程序信息
   *
   * 用于向 Vulkan 驱动描述当前应用及引擎的基本信息，
   * 主要用于驱动内部统计和调试，对功能无直接影响。
   */
  ApplicationInfo application_info {
    default_application_name,     // 应用程序名称
    default_application_version,  // 应用程序版本
    default_engin_name,           // 引擎名称
    default_application_version,  // 引擎版本
    vulkan_api_version            // 使用的 Vulkan API 版本
  };

  /**
   * @brief Instance 启用的扩展列表
   *
   * - 默认从 GLFW 获取创建 Surface 所必需的扩展
   * - 在 Debug 模式下会额外添加调试扩展
   */
  std::vector<const char*> extensions = glfw::get_vk_extensions();

  /**
   * @brief Instance 启用的 Validation Layers
   *
   * - 当前仅启用 Khronos 官方 validation layer
   * - 仅在 Debug 模式下实际生效
   */
  static constexpr const char* layers[] = {
    vulkan_layer_khronos_validation
  };

  /**
   * @brief 构造 InstanceCreateInfo
   *
   * 根据编译模式（Debug / Release）自动配置：
   * - Validation Layer
   * - Debug 扩展
   * - Instance 创建所需的扩展与层信息
   */
  instance_create_info() {
    if constexpr (is_debug_mode) {
      /// 启用 validation layer 数量
      enabledLayerCount = 1;

      /// 添加调试扩展（如 VK_EXT_debug_utils）
      extensions.push_back(vulkan_debug_extension_name);

      /// 输出当前环境所需的 Instance extensions（调试用）
      yuri::info("GLFW 当前环境所需 extensions: {}", extensions.size());
      for (auto& instanceExtension : extensions) {
        yuri::info("  extension {}", instanceExtension);
      }
    }

    /// Vulkan 结构体类型标识
    sType = StructureType::eInstanceCreateInfo;

    /// 指向应用程序信息结构体
    pApplicationInfo = &application_info;

    /// 启用的 Layer 名称数组
    ppEnabledLayerNames = layers;

    /// 启用的 Instance 扩展数量
    enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());

    /// 启用的 Instance 扩展名称列表
    ppEnabledExtensionNames = extensions.data();
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
  Queue queue;                     // 队列
  SurfaceKHR surface_khr;          // surface

// todo: 未完全替换c api前暂时public
public:
  vulkan_instance() {
    if (const auto res = createInstance(&info, nullptr, this); res != Result::eSuccess) {
      throw std::runtime_error(std::format(
        "创建vulkan_instance 失败, 错误码: {}", to_string(res))
      );
    }

    auto [physical_device, index] = pick_physical_device_and_graphics_queue(*this);
    this->physical_device = physical_device;
    this->logic_device = create_logical_device(physical_device, index);
    this->queue = logic_device.getQueue(index, index);

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

// 初始化application
auto application = std::make_unique<glfw::application>();

// 初始化vulkan_instance
auto global_vulkan_instance_ = std::make_shared<vulkan_instance>();
export auto global_vulkan_instance = global_vulkan_instance_.get();