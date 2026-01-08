//
// Created by yuri on 2026/1/4.
//

/**
 * vulkan 配置
 */
export module vulkan_config;

import std;

export namespace vk {

constexpr auto default_application_name = "yuri"; // 默认程序名称
constexpr auto default_engin_name = "no-engine";  // 默认引擎名称
constexpr auto default_application_major = 1;     // 默认版本
constexpr auto default_application_minor = 0;
constexpr auto default_application_patch = 0;

// 分配的指令buffer数量
constexpr auto command_buffer_size = 1;

// 等待fence最长时间
constexpr auto wait_fence_timeout = std::numeric_limits<std::uint64_t>::max();

}