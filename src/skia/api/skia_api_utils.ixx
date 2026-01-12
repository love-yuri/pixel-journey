//
// Created by yuri on 2026/1/12.
//

export module skia.api:utils;

import :core;
import vulkan.api;
import glfw.api;


export namespace skia {

/**
 * 创建VkImageInfo
 * @param image vulkan image
 * @param extent 宽高
 * @param queue_family_index index
 * @param context 上下文指针
 * @return info
 */
sk_sp<SkSurface> create_surface(
  const vk::Image * image,
  const vk::Extent2D &extent,
  const std::uint32_t queue_family_index,
  GrDirectContext* context
) {
  const SkSurfaceProps props(0, kUnknown_SkPixelGeometry);

  GrVkImageInfo imageInfo{};
  imageInfo.fImage = *image;
  imageInfo.fImageLayout = vk::VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.fFormat = vk::VK_FORMAT_B8G8R8A8_UNORM;
  imageInfo.fImageTiling = vk::VK_IMAGE_TILING_OPTIMAL;
  imageInfo.fLevelCount = 1;
  imageInfo.fCurrentQueueFamily = queue_family_index;

  const auto backendRT = GrBackendRenderTargets::MakeVk(extent.width, extent.height, imageInfo);
  const auto surface = SkSurfaces::WrapBackendRenderTarget(
    context,
    backendRT,
    kTopLeft_GrSurfaceOrigin,
    kBGRA_8888_SkColorType,
    nullptr,
    &props
  );

  if (!surface) {
    throw std::runtime_error("创建skia surface失败!");
  }

  return surface;
}



}