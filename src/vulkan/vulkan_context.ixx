/**
 * vulkan上下文模块
 */
export module vulkan.context;

import vulkan.api;
import std;

using namespace vk;

class vulkan_instance;

/**
 * 全局vulkan上下文
 */
struct vulkan_context {
  Instance *instance;
  PhysicalDevice* physical_device;
  Device* device;
  Queue* graphics_queue;
};


/**
 * window级别上下文
 */
export struct window_content {

};


export vulkan_context vulkan_context{};

// // 初始化application
// auto application = std::make_unique<glfw::application>();
//
// // 初始化vulkan_instance
// auto global_vulkan_instance_ = std::make_shared<vulkan_instance>();
// export auto global_vulkan_instance = global_vulkan_instance_.get();
