#include <random>
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
#include "include/private/chromium/GrVkSecondaryCBDrawContext.h"

import std;
import vulkan;
import glfw;
import yuri_log;
import skia.font;

PFN_vkVoidFunction GetProc(const char *name, VkInstance instance, VkDevice device) {
  if (device != VK_NULL_HANDLE) {
    return vkGetDeviceProcAddr(device, name);
  }
  return vkGetInstanceProcAddr(instance, name);
}

class FPSCounter {
  int frameCount = 0;
  double lastTime = 0.0;
  double fps = 0.0;

public:
  void update(const double currentTime) {
    frameCount++;

    // 每秒钟计算一次FPS
    if (currentTime - lastTime >= 1.0) {
      fps = frameCount / (currentTime - lastTime);
      frameCount = 0;
      lastTime = currentTime;
    }
  }

  [[nodiscard]] double getFPS() const { return fps; }
};

double getCurrentTime() {
  // 获取当前时间（秒，高精度）
  auto now = std::chrono::high_resolution_clock::now();
  auto duration = now.time_since_epoch();
  return std::chrono::duration<double>(duration).count();
}

int main() {
  skgpu::VulkanBackendContext vkContext;
  vkContext.fInstance = vulkan_context->instance;
  vkContext.fPhysicalDevice = vulkan_context->physical_device;
  vkContext.fDevice = vulkan_context->logic_device;
  vkContext.fGraphicsQueueIndex = vulkan_context->queue_family_index;
  vkContext.fQueue = vulkan_context->queue;
  vkContext.fGetProc = GetProc;

  GrContextOptions options;
  options.fSuppressPrints = true;

  auto grContext = GrDirectContexts::MakeVulkan(vkContext, options);

  glfw::glfw_window gw = {200, 200, "yuri"};
  gw.show();

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution dis(0.0f, 800.0f);

  sk_sp<SkTypeface> typeface = skia::font::load_from_file(R"(E:\love-yuri\journal-kmp\composeApp\src\commonMain\composeResources\font\MapleMono-NF-CN-Medium.ttf)");
  SkFont font(typeface, 24);

  // 使用 Vulkan 专用函数
  auto presentState = skgpu::MutableTextureStates::MakeVulkan(
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      vulkan_context->queue_family_index
  );

  SkPaint paint;
  paint.setColor(SK_ColorRED);
  paint.setAntiAlias(true);

  FPSCounter fpsCounter;
  SkSurfaceProps props(0, kUnknown_SkPixelGeometry);
  while (!gw.should_close()) {
    const auto frame = gw.acquire_next_frame();
    frame->begin_frame();

    fpsCounter.update(getCurrentTime());

    GrVkImageInfo imageInfo{};
    imageInfo.fImage = *frame->image;
    imageInfo.fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.fFormat = VK_FORMAT_B8G8R8A8_UNORM;
    imageInfo.fImageTiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.fLevelCount = 1;
    imageInfo.fCurrentQueueFamily = vulkan_context->queue_family_index;

    GrBackendRenderTarget backendRT = GrBackendRenderTargets::MakeVk(
      gw.width(),
      gw.height(),
      imageInfo
    );

    auto surface = SkSurfaces::WrapBackendRenderTarget(
      grContext.get(),
      backendRT,
      kTopLeft_GrSurfaceOrigin,
      kBGRA_8888_SkColorType,
      nullptr,
      &props
    );

    if (surface == nullptr) {
      yuri::error("surface is null!");
      return 0;
    }

    SkCanvas* canvas = surface->getCanvas();
    canvas->clear(SK_ColorWHITE);

    canvas->drawRect(SkRect::MakeXYWH(dis(gen), dis(gen), 100, 100), paint);
    canvas->drawString(std::format("FPS: {:.1f}", fpsCounter.getFPS()).c_str(), 100, 830, font, paint);

    // Flush 并转换到呈现布局
    grContext->flush(surface.get(), {}, &presentState);
    grContext->submit(GrSyncCpu::kNo);

    frame->submit();
    frame->present();

    glfw::glfwPollEvents();
  }
}