//
// Created by yuri on 2026/1/12.
//

export module skia.api:utils;

import :core;
import vulkan.api;
import glfw.api;
import yuri_log;

export namespace skia {

/**
 * 从编码后的图片数据解码 SkImage
 * @param data 图片二进制数据
 * @return 解码后的图片，失败时为空
 */
sk_sp<SkImage> decodeImage(const std::string_view data) {
  if (data.empty()) {
    return nullptr;
  }

  const auto sk_data = SkData::MakeWithCopy(data.data(), data.size());
  return images::DeferredFromEncodedData(sk_data);
}


/**
 * 将Vulkan格式映射到Skia颜色类型
 * @param format Vulkan格式
 * @return Skia颜色类型
 */
SkColorType color_type_from_format(const vk::Format format) {
  switch (format) {
  case vk::Format::eB8G8R8A8Unorm:
  case vk::Format::eB8G8R8A8Srgb:
    return kBGRA_8888_SkColorType;
  case vk::Format::eR8G8B8A8Unorm:
  case vk::Format::eR8G8B8A8Srgb:
    return kRGBA_8888_SkColorType;
  default:
    throw std::runtime_error(std::format("创建 Skia Surface 失败: 不支持的 swapchain format={}", vk::to_string(format)));
  }
}

/**
 * 创建VkImageInfo
 * @param image vulkan image
 * @param extent 宽高
 * @param format swapchain实际格式
 * @param queue_family_index index
 * @param context 上下文指针
 * @return info
 */
sk_sp<SkSurface> create_surface(
  const vk::Image * image,
  const vk::Extent2D &extent,
  const vk::Format format,
  const std::uint32_t queue_family_index,
  GrDirectContext* context
) {
  if (context == nullptr) {
    throw std::runtime_error("创建 Skia Surface 失败: GrDirectContext 为空");
  }

  const auto color_type = color_type_from_format(format);
  const SkSurfaceProps props(0, kRGB_H_SkPixelGeometry);

  GrVkImageInfo imageInfo{};
  imageInfo.fImage = *image;
  imageInfo.fImageLayout = vk::VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.fFormat = static_cast<vk::VkFormat>(format);
  imageInfo.fImageTiling = vk::VK_IMAGE_TILING_OPTIMAL;
  imageInfo.fLevelCount = 1;
  imageInfo.fCurrentQueueFamily = queue_family_index;

  const auto backendRT = GrBackendRenderTargets::MakeVk(
    static_cast<int>(extent.width),
    static_cast<int>(extent.height),
    imageInfo
  );
  if (!backendRT.isValid()) {
    throw std::runtime_error(std::format(
      "创建 Skia Surface 失败: GrBackendRenderTarget 无效, extent={}x{}, format={}, layout=VK_IMAGE_LAYOUT_UNDEFINED, queueFamily={}",
      extent.width,
      extent.height,
      vk::to_string(format),
      queue_family_index
    ));
  }

  const auto surface = SkSurfaces::WrapBackendRenderTarget(
    context,
    backendRT,
    kTopLeft_GrSurfaceOrigin,
    color_type,
    nullptr,
    &props
  );

  if (!surface) {
    throw std::runtime_error(std::format(
      "创建 Skia Surface 失败: WrapBackendRenderTarget 返回空, extent={}x{}, format={}, layout=VK_IMAGE_LAYOUT_UNDEFINED, queueFamily={}",
      extent.width,
      extent.height,
      vk::to_string(format),
      queue_family_index
    ));
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
