export module vulkan.detail:swapchain_create_info;

import vulkan.api;
import std;

export namespace vk::detail {

/**
 * @brief Swapchain 创建信息封装
 *
 * vulkan_swapchain_create_info 继承自 vk::SwapchainCreateInfoKHR，
 * 用于简化 Swapchain 创建参数的填写过程。
 *
 * 当前实现中：
 * - 仅保留创建 Swapchain 所必需的核心参数
 * - 一些常用但变化不大的参数被写死为推荐或最常见配置
 * - 适用于单队列族、普通窗口渲染的场景
 *
 * 若后续需要支持：
 * - 多队列族并发访问
 * - 可变 present mode
 * - 不同 usage / alpha / clipping 行为
 * 可在此结构基础上继续扩展
 */
struct swapchain_create_info : SwapchainCreateInfoKHR {

  /**
   * @brief 构造 SwapchainCreateInfoKHR
   *
   * @param surface_khr
   * Vulkan 渲染目标 Surface
   * - 由平台相关接口创建（Wayland / XCB / Win32 等）
   * - Swapchain 将向该 Surface 进行呈现
   *
   * @param image_count
   * Swapchain 中图像的最小数量（minImageCount）
   * - 通常为 2（双缓冲）或 3（三缓冲）
   * - 必须满足 SurfaceCapabilities 中的限制
   *
   * @param format
   * Swapchain 图像的像素格式（imageFormat）
   * - 例如 vk::Format::eB8G8R8A8Srgb
   * - 必须为物理设备支持的 Surface 格式之一
   *
   * @param color_space
   * Swapchain 使用的颜色空间（imageColorSpace）
   * - 常见为 vk::ColorSpaceKHR::eSrgbNonlinear
   *
   * @param image_extent
   * Swapchain 图像分辨率
   * - {} 顺序：
   *   - [0] width  ：图像宽度
   *   - [1] height ：图像高度
   *
   * @param pre_transform
   * Surface 变换方式（preTransform）
   * - 通常直接使用 surface capabilities 的 currentTransform
   * - 用于处理旋转、翻转等窗口变换
   */
  explicit swapchain_create_info(
    const SurfaceKHR surface_khr,
    const std::uint32_t image_count,
    const Format format,
    const ColorSpaceKHR color_space,
    const Extent2D &image_extent,
    const SurfaceTransformFlagBitsKHR pre_transform
  ) {
    /// 目标呈现 Surface
    surface = surface_khr;

    /// Swapchain 中图像的最小数量
    minImageCount = image_count;

    /// 图像像素格式
    imageFormat = format;

    /// 颜色空间
    imageColorSpace = color_space;

    /// Swapchain 图像分辨率
    imageExtent = image_extent;

    /// 图像数组层数（非立体渲染，固定为 1）
    imageArrayLayers = 1;

    /**
     * 图像用途：
     * - eColorAttachment：作为渲染目标
     * - eTransferDst   ：允许拷贝/清屏操作
     */
    imageUsage =
      ImageUsageFlagBits::eColorAttachment |
      ImageUsageFlagBits::eTransferDst;

    /// 独占模式，适用于单队列族访问
    imageSharingMode = SharingMode::eExclusive;

    /// Surface 预变换方式
    preTransform = pre_transform;

    /// 合成 Alpha 模式（忽略窗口透明度）
    compositeAlpha = CompositeAlphaFlagBitsKHR::eOpaque;

    /// 呈现模式：FIFO（垂直同步，保证可用）
    presentMode = PresentModeKHR::eFifo;

    /// 被遮挡像素是否可以被丢弃（通常开启）
    clipped = true;
  }
};

}