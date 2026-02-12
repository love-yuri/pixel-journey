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
#include <include/core/SkTextBlob.h>
#include <include/core/SkShader.h>
#include <modules/skresources/include/SkResources.h>
#include <modules/svg/include/SkSVGDOM.h>
#include <modules/skshaper/utils/FactoryHelpers.h>
#include <include/core/SkStream.h>
export module skia.api:core;

import std;

/**
 * 默认空间
 */
export namespace skia {

using ::SkPathFillType;
using ::SkTextBlob;
using ::SkPoint;
using ::SkRect;
using ::SkMatrix;
using ::SkPath;
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
consteval SkColor ColorFromARGB(const U8CPU a,const U8CPU r, const U8CPU g, const U8CPU b) noexcept {
  return SkColorSetARGB(a, r, g, b);
}

} // namespace skia

/**
 * skgpu
 */
export namespace skia::gpu {

using skgpu::VulkanBackendContext;
using skgpu::MutableTextureState;

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

}

export namespace skia::resources {

using skresources::ImageDecodeStrategy;
using skresources::FileResourceProvider;
using skresources::DataURIResourceProviderProxy;

}

export namespace skia::shapers {

using SkShapers::BestAvailable;

}
