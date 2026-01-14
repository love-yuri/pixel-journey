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

  SkPaint paint = [] {
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
  ui::render::RenderBorder border;
  void render(SkCanvas *canvas) override {
    fpsCounter.update(getCurrentTime());

    canvas->clear(skia_colors::white);
    if (current_point.equals(0, 0)) {
      border.radius = 20;
      current_point = {
        (m_width / 2),
        (m_height / 2)
      };

      border.rect = SkRect::MakeLTRB(
        current_point.x() - r,
        current_point.y() - r,
          current_point.x() + r,
        current_point.y() + r
      );
    }

    const SkFont font(typeface, 24);
    float r = 220;
    if (is_clicked) {
      r = 120;
    }
    canvas->drawCircle(current_point, r, paint);
    canvas->drawLine(50, 0, 50, m_height, paint);
    border.render(canvas);
    canvas->drawString(std::format("FPS: {:.1f}", fpsCounter.getFPS()).c_str(), 100, 830, font, paint);
    canvas->drawString(std::format("current: {:.1f} {:.1f}",current_point.x(), current_point.y()).c_str(), 100, 530, font, paint);
  }

  void on_mouse_enter() override {

  }

  void on_mouse_leave() override {
    yuri::info("鼠标移出 {}, {}", current_point.x(), current_point.y());
  }

  void on_mouse_move(const float x, const float y) override {
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

  void on_mouse_left_pressed() override {
    is_clicked = true;
    last_point = get_cursor_position();
  }

  void on_mouse_left_released() override {
    is_clicked = false;
  }

  void on_resize(const int width, const int height) override {
    glfw::Window::on_resize(width, height);
    current_point = {
      m_width / 2,
      (m_height / 2)
    };
  }
};

int main() {
  Window gw = {1200, 800, "yuri"};
  gw.show();
}