#include <random>

import std;
import vulkan;
import glfw;
import yuri_log;
import skia.font;
import skia;

using namespace skia;

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

    auto surface = frame->sk_surface;

    SkCanvas* canvas = surface->getCanvas();
    canvas->clear(SKColorWhite);

    SkPoint center = {
      static_cast<float>(gw.width() / 2),
      static_cast<float>(gw.height() / 2)
    };

    canvas->drawCircle(center, 220, paint);
    canvas->drawString(std::format("FPS: {:.1f}", fpsCounter.getFPS()).c_str(), 100, 830, font, paint);

    // Flush 并转换到呈现布局
    vulkan_context->skia_direct_context->flush(surface, {}, &presentState);
    vulkan_context->skia_direct_context->submit(GrSyncCpu::kNo);

    frame->submit();
    frame->present();

    glfw::glfwPollEvents();
  }
}