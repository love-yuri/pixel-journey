#include <random>

import std;
import vulkan;
import glfw;
import yuri_log;
import skia;
import ui;

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

class Window: public glfw::Window {
  using glfw::Window::Window;
  FPSCounter fpsCounter;
  sk_sp<SkTypeface> typeface = font::load_from_file(font::default_font_path);

  SkPaint paint2 = [] {
    SkPaint paint;
    paint.setColor(skia_colors::gray);
    paint.setAntiAlias(true);
    paint.setStrokeWidth(8);
    return paint;
  }();

public:
  SkPoint current_point = {0, 0 };
  SkPoint last_point = {0, 0};
  bool is_clicked = false;
  bool is_hover = false;
  float r = 220;
  ui::widgets::Button* button;

  ~Window() override {
    yuri::info("~Window");
  }

  Window(): glfw::Window(800, 800) {
    button = new ui::widgets::Button("测试", this);

    yuri::info("child: {}", children_.size());
  }

  void paint(SkCanvas *canvas) override {
    const SkFont font(typeface, 24);
    float r = 220;
    if (is_clicked) {
      r = 43.75;
    }

    canvas->drawCircle(current_point, r, paint2);
    canvas->drawLine(50, 0, 50, height_, paint2);

    canvas->drawString(std::format("FPS: {:.1f}", fpsCounter.getFPS()).c_str(), 100, 830, font, paint2);
    canvas->drawString(std::format("current: {:.1f} {:.1f}",current_point.x(), current_point.y()).c_str(), 100, 530, font, paint2);
  }

  void render(SkCanvas *canvas) override {
    fpsCounter.update(getCurrentTime());

    canvas->clear(skia_colors::white);
    Widget::render(canvas);
  }

  void onMouseEnter() override {

  }

  void onMouseLeave() override {
    delete button;
    button = nullptr;
    yuri::info("鼠标移出 {}, {}", current_point.x(), current_point.y());
  }

  void onMouseMove(const float x, const float y) override {
    const auto rect = SkRect::MakeLTRB(
      current_point.x() - r,
      current_point.y() - r,
      current_point.x() + r,
      current_point.y() + r
    );
    if (rect.contains(x, y)) {
      is_hover = true;
    } else {
      is_hover = false;
    }
    if (is_clicked) {
      const auto new_x = x - last_point.x();
      const auto new_y = y - last_point.y();
      current_point.set(current_point.x() + new_x, current_point.y() + new_y);
      last_point.set(x, y);
    }
  }

  void onMouseLeftPressed() override {
    is_clicked = true;
    last_point = getCursorPosition();
  }

  void onMouseLeftReleased() override {
    is_clicked = false;
    const auto p = getCursorPosition();
    hit_test_grid.check(p.x(), p.y());
  }

  void onResize(const int width, const int height) override {
    glfw::Window::onResize(width, height);
    current_point = {
      width_ / 2,
      (height_ / 2)
    };
  }
};

int main() {
  Window gw = {};
  gw.show();
}