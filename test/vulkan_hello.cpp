#include <random>

import std;
import vulkan;
import glfw;
import yuri_log;
import skia.font;
import skia_api;

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
  vkContext.fGetProc = vk::vulkan_get_proc;

  GrContextOptions options;
  options.fSuppressPrints = true;

  auto grContext = MakeVulkan(vkContext, options);

  glfw::glfw_window gw = {200, 200, "yuri"};
  gw.show();

  std::random_device rd;
  std::mt19937 gen(rd());

  sk_sp<SkTypeface> typeface = skia::font::load_from_file(skia::font::default_font_path);
  SkFont font(typeface, 24);

  // 使用 Vulkan 专用函数
  auto presentState = skgpu::MutableTextureStates::MakeVulkan(
      vk::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      vulkan_context->queue_family_index
  );

  SkPaint paint;
  paint.setColor(SKColorRed);
  paint.setAntiAlias(true);

  FPSCounter fpsCounter;
  SkSurfaceProps props(0, kUnknown_SkPixelGeometry);
  while (!gw.should_close()) {
    const auto frame = gw.acquire_next_frame();
    frame->begin_frame();

    fpsCounter.update(getCurrentTime());

    GrVkImageInfo imageInfo{};
    imageInfo.fImage = *frame->image;
    imageInfo.fImageLayout = vk::VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.fFormat = vk::VK_FORMAT_B8G8R8A8_UNORM;
    imageInfo.fImageTiling = vk::VK_IMAGE_TILING_OPTIMAL;
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

    if (!surface) {
      yuri::error("surface is null!");
      return 0;
    }

    SkCanvas* canvas = surface->getCanvas();
    canvas->clear(SKColorWhite);

    SkPoint center = {
      static_cast<float>(gw.width() / 2),
      static_cast<float>(gw.height() / 2)
    };

    canvas->drawCircle(center, 220, paint);
    canvas->drawString(std::format("FPS: {:.1f}", fpsCounter.getFPS()).c_str(), 100, 830, font, paint);

    // Flush 并转换到呈现布局
    grContext->flush(surface.get(), {}, &presentState);
    grContext->submit(GrSyncCpu::kNo);

    frame->submit();
    frame->present();

    glfw::glfwPollEvents();
  }
}