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

  const auto backendRT = GrBackendRenderTargets::MakeVk(
    static_cast<int>(extent.width),
    static_cast<int>(extent.height),
    imageInfo
  );
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

class SkPaintBuilder {
public:
  SkPaintBuilder() = default;

  // 设置颜色
  SkPaintBuilder&& setColor(const SkColor color)&& {
    fPaint.setColor(color);
    return std::move(*this);
  }

  // 设置抗锯齿
  SkPaintBuilder&& setAntiAlias(const bool antiAlias)&& {
    fPaint.setAntiAlias(antiAlias);
    return std::move(*this);
  }

  // 设置笔触宽度
  SkPaintBuilder&& setStrokeWidth(const float width)&& {
    fPaint.setStrokeWidth(width);
    return std::move(*this);
  }

  // 设置笔触样式（实线/虚线等）
  SkPaintBuilder&& setStrokeStyle(const SkPaint::Style style)&& {
    fPaint.setStyle(style);
    return std::move(*this);
  }

  // 设置 Shader（渐变/图片等）
  SkPaintBuilder&& setShader(sk_sp<SkShader> shader)&& {
    fPaint.setShader(std::move(shader));
    return std::move(*this);
  }

  // 设置透明度（0-255）
  SkPaintBuilder&& setAlpha(const U8CPU alpha)&& {
    fPaint.setAlpha(alpha);
    return std::move(*this);
  }

  // 设置混合模式（BlendMode）
  SkPaintBuilder&& setBlendMode(const SkBlendMode mode) && {
    fPaint.setBlendMode(mode);
    return std::move(*this);
  }

  // 构造
  SkPaint build() && {
    return std::move(fPaint);
  }

  // 禁止对左值调用 build()
  SkPaint build() & = delete;

private:
  SkPaint fPaint {};
};

}