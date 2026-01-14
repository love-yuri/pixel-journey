export module ui.render:text;

import :base;
import skia.resource;
import yuri_log;
import std;

using namespace skia;

export namespace ui::render {

class RenderText : public RenderNode {
  SkFont font = font::default_font;
  std::string text;

  SkPaint paint = [this] {
    SkPaint paint;
    paint.setColor(skia_colors::black);
    paint.setStrokeWidth(2);
    paint.setAntiAlias(true);
    return paint;
  }();

public:

  void set_font_size(const float size) {
    font.setSize(size);
  }

  void set_color(const SkColor color) {
    paint.setColor(color);
  }

  void set_text(const std::string_view text) {
    this->text = text;
  }

  void render(SkCanvas* canvas) override {
    SkRect bounds;
    font.measureText(text.c_str(), text.size(), SkTextEncoding::kUTF8, &bounds);

    // 水平居中
    const float startX = rect.x() + (rect.width() - bounds.width()) / 2;

    // 垂直居中
    const float baselineY = rect.y() + rect.height() / 2 - bounds.centerY();

    // 绘制文本（无需再调整 y 坐标）
    canvas->drawSimpleText(
        text.c_str(), text.size(),
        SkTextEncoding::kUTF8,
        startX, baselineY,
        font,
        paint
    );
  }
};

} // namespace ui::render
