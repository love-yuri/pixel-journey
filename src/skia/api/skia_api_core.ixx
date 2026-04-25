//
// Created by yuri on 2026/1/12.
//
// ReSharper disable CppUnusedIncludeDirective
module;
#include <include/core/SkFontMgr.h>
#if defined(_WIN32)
#include <include/ports/SkTypeface_win.h>
#else
#include <include/ports/SkFontScanner_FreeType.h>
#include <include/ports/SkFontMgr_fontconfig.h>
#include <include/core/SkTypeface.h>
#endif
#include <include/core/SkCanvas.h>
#include <include/gpu/vk/VulkanBackendContext.h>
#include <include/gpu/ganesh/vk/GrVkDirectContext.h>
#include <include/gpu/ganesh/vk/GrVkBackendSurface.h>
#include <include/gpu/ganesh/GrBackendSurface.h>
#include <include/gpu/ganesh/vk/GrVkTypes.h>
#include <include/gpu/vk/VulkanMutableTextureState.h>
#include <include/gpu/ganesh/SkSurfaceGanesh.h>
#include <include/gpu/ganesh/GrRecordingContext.h>
#include <include/core/SkColorSpace.h>
#include <include/gpu/MutableTextureState.h>
#include <include/gpu/ganesh/GrDirectContext.h>
#include <include/core/SkFont.h>
#include <include/core/SkSurface.h>
#include <include/core/SkFontMetrics.h>
#include <include/core/SkColor.h>
#include <include/core/SkRRect.h>
#include <include/core/SkImage.h>
#include <include/core/SkSamplingOptions.h>
#include <include/core/SkTextBlob.h>
#include <include/core/SkColorFilter.h>
#include <include/core/SkShader.h>
#include <modules/skresources/include/SkResources.h>
#include <modules/svg/include/SkSVGDOM.h>
#include <modules/skshaper/utils/FactoryHelpers.h>
#include <include/core/SkStream.h>
#include <include/core/SkPathBuilder.h>
export module skia.api:core;

import std;

/**
 * 默认空间
 */
export namespace skia {

using ::SkColorFilters;
using ::SkColorFilter;
using ::SkSamplingOptions;
using ::SkImageInfo;
using ::SkImage;
using ::SkPathFillType;
using ::SkTextBlob;
using ::SkPoint;
using ::SkRect;
using ::SkMatrix;
using ::SkPath;
using ::SkPathBuilder;
using ::SkRRect;
using ::SkStream;
using ::SkSurface;
using ::SkCanvas;
using ::GrContextOptions;
using ::SkTypeface;
using GrDirectContexts::MakeVulkan;
using ::SkSurfaceProps;
using ::sk_sp;
using ::SkFont;
using ::SkPaint;
using ::kUnknown_SkPixelGeometry;
using ::GrVkImageInfo;
using ::GrBackendRenderTarget;
using ::kTopLeft_GrSurfaceOrigin;
using ::kBGRA_8888_SkColorType;
using ::GrSyncCpu;
using ::GrDirectContext;
using ::SkFontMgr;
using ::SkColor;
using ::SkTextEncoding;
using ::SkFontMetrics;
using ::SkShader;
using ::U8CPU;
using ::SkBlendMode;
using ::SkString;
using ::SkSVGDOM;
using ::SkSize;
using ::SkFILEStream;
using ::SkScalar;

/* 字体相关 */
#if defined(_WIN32)
using ::SkFontMgr_New_DirectWrite;
#else
using ::SkFontMgr_New_FontConfig;
using ::SkFontScanner_Make_FreeType;
#endif

/**
 * 导出内联函数
 */
constexpr SkColor ColorFromARGB(const U8CPU a,const U8CPU r, const U8CPU g, const U8CPU b) noexcept {
  return SkASSERT(a <= 255 && r <= 255 && g <= 255 && b <= 255), (a << 24 | r << 16 | g << 8 | b << 0);
}

/**
 * 创建心形路径
 * @param cx 中心x坐标
 * @param cy 中心y坐标
 * @param size 尺寸（宽度）
 * @return 心形SkPath
 */
SkPath MakeHeartPath(float cx, float cy, float size) {
  const float w = size;
  const float h = size;
  const float left = cx - w * 0.5f;
  const float top = cy - h * 0.5f;
  SkPathBuilder builder;
  builder.moveTo(SkPoint::Make(cx, top + h));
  builder.cubicTo(SkPoint::Make(cx - w * 0.22f, top + h * 0.79f),
                  SkPoint::Make(left, top + h * 0.61f),
                  SkPoint::Make(left, top + h * 0.39f));
  builder.cubicTo(SkPoint::Make(left, top + h * 0.18f),
                  SkPoint::Make(left + w * 0.14f, top),
                  SkPoint::Make(left + w * 0.32f, top));
  builder.cubicTo(SkPoint::Make(left + w * 0.41f, top),
                  SkPoint::Make(cx, top + h * 0.06f),
                  SkPoint::Make(cx, top + h * 0.14f));
  builder.cubicTo(SkPoint::Make(cx, top + h * 0.06f),
                  SkPoint::Make(left + w * 0.59f, top),
                  SkPoint::Make(left + w * 0.68f, top));
  builder.cubicTo(SkPoint::Make(left + w * 0.86f, top),
                  SkPoint::Make(left + w, top + h * 0.18f),
                  SkPoint::Make(left + w, top + h * 0.39f));
  builder.cubicTo(SkPoint::Make(left + w, top + h * 0.61f),
                  SkPoint::Make(cx + w * 0.22f, top + h * 0.79f),
                  SkPoint::Make(cx, top + h));
  builder.close();
  return builder.detach();
}

/**
 * 将color转hsv
 * @param color  原颜色
 * @param hsv hsv数组
 */
void ColorToHSV(const SkColor color, SkScalar hsv[3]) { return SkColorToHSV(color, hsv); }

/**
 * hsv转color
 */
SkColor HSVToColor(const U8CPU alpha, const SkScalar hsv[3]) { return SkHSVToColor(alpha, hsv); }

} // namespace skia

/**
 * skgpu
 */
export namespace skia::gpu {

using skgpu::VulkanBackendContext;
using skgpu::MutableTextureState;
using skgpu::Budgeted;

} // namespace skia::skgpu

/**
 * MutableTextureStates
 */
export namespace skia::gpu::MutableTextureStates {

using skgpu::MutableTextureStates::MakeVulkan;

}

/**
 * GrBackendRenderTargets
 */
export namespace skia::GrBackendRenderTargets {

using ::GrBackendRenderTargets::MakeVk;

}

/**
 * SkSurfaces
 */
export namespace SkSurfaces {

using SkSurfaces::WrapBackendRenderTarget;
using SkSurfaces::Raster;
using SkSurfaces::RenderTarget;

}

export namespace skia::resources {

using skresources::ImageDecodeStrategy;
using skresources::FileResourceProvider;
using skresources::DataURIResourceProviderProxy;

}

export namespace skia::shapers {

using SkShapers::BestAvailable;

}
