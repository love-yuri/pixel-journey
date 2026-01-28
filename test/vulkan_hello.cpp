#include <random>

import std;
import vulkan;
import glfw;
import yuri_log;
import skia;
import ui;

using namespace skia;
using namespace ui::layout;
using namespace ui::widgets;

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

class Window: public glfw::Window {
  using glfw::Window::Window;
  FPSCounter fpsCounter;

  SkPaint sk_paint_ = [] {
    SkPaint paint;
    paint.setColor(skia_colors::gray);
    paint.setAntiAlias(true);
    paint.setStrokeWidth(8);
    return paint;
  }();

public:
  Button* button = nullptr;

  void onClick() {
    // animation_manager->start(200.f, 300.f, 400, this, ui::animation::memberThunk<Widget, float, &Widget::setPadding>);
  }

  Window(): glfw::Window(800, 800) {
    setPadding(200);
    button = new Button("测试", this);
    button->clicked.connect<&Window::onClick>(this);
    // layout_ = std::make_unique<VBoxLayout<Widget>>(this);
    button->setMaxHeight(50);
    button->resize(100, 50);
    // const auto text = new Text("哈哈哈哈", this);
    // text->setAlignment(Alignment::Center);
    // text->setMaxHeight(50);
  }

  void paint(SkCanvas *canvas) override {
    canvas->drawString(std::format("FPS: {:.1f}", fpsCounter.getFPS()).c_str(), 20, 30, font::default_font, sk_paint_);
    canvas->drawString(std::format("current: {:.1f} {:.1f}", cursor_x, cursor_y).c_str(), 20, 130, font::default_font, sk_paint_);
  }

  void render(SkCanvas *canvas) override {
    fpsCounter.update(getCurrentTime());

    canvas->clear(skia_colors::white);

    Widget::render(canvas);
  }
};

int main() {
  Window gw = {};
  gw.show();
}