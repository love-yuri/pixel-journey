module;
// ReSharper disable once CppUnusedIncludeDirective
#include "include/private/base/SkFloatingPoint.h"
export module ui.render:text;

import :base;
import skia.resource;
import ui.layout;
import yuri_log;
import std;

using namespace skia;
using namespace ui::layout;

export namespace ui::render {

class RenderText : public RenderNode {
  SkRect font_rect;                 // 文本bound
  float x = 0.0;                    // 渲染基线 x
  float y = 0.0;                    // 渲染基线 y
  SkFont font = font::default_font; // 字体
  std::string text;                 // label
  SkPaint paint = PaintDesc{};      // paint
  sk_sp<SkTextBlob> blob;           // 文本blob

public:
  Alignment alignment = Alignment::TopLeft; // 对齐方式

  /**
   * 构造一个文本节点
   * @param text 显示的文本
   * @param rect 所占空间
   */
  RenderText(std::string_view text, const SkRect& rect);

  /**
   * 设置字体大小
   * @param size 字体大小
   */
  void set_font_size(float size);

  /**
   * 设置字体颜色
   */
  void set_color(SkColor color);

  /**
   * 设置文本
   */
  void set_text(std::string_view text);

  /**
   * render
   */
  void render(SkCanvas *canvas) override;

private:
  /**
   * 重新计算文本、位置等信息
   */
  void re_calculate();
};

RenderText::RenderText(const std::string_view text, const SkRect& rect): RenderNode(rect), text(text) {
  re_calculate();
}

void RenderText::set_font_size(const float size) {
  font.setSize(size);
  re_calculate();
}

void RenderText::set_color(const SkColor color) {
  paint.setColor(color);
}

void RenderText::set_text(const std::string_view text) {
  this->text = text;
  re_calculate();
}

void RenderText::render(SkCanvas *canvas) {
  if (visible) {
    canvas->drawTextBlob(blob, x, y, paint);
  }
}

void RenderText::re_calculate() {
  // 计算bound
  font.measureText(this->text.data(), this->text.size(), SkTextEncoding::kUTF8, &font_rect);

  // 计算起点x
  if (alignment & Alignment::Left) {
    x = rect.left() - font_rect.left();
  } else if (alignment & Alignment::Right) {
    x = rect.right() - font_rect.right();
  } else if (alignment & Alignment::HCenter) {
    x = rect.centerX() - font_rect.centerX();
  }

  // 计算起点y
  if (alignment & Alignment::Top) {
    y = rect.top() - font_rect.top();
  } else if (alignment & Alignment::Bottom) {
    y = rect.bottom() - font_rect.bottom();
  } else if (alignment & Alignment::VCenter) {
    y = rect.centerY() - font_rect.centerY();
  }

  // 计算blob
  blob = SkTextBlob::MakeFromText(
    text.data(),
    text.size(),
    font,
    SkTextEncoding::kUTF8
  );
}

} // namespace ui::render
