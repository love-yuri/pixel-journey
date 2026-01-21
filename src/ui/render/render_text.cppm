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
  using RenderNode::RenderNode;
  SkRect font_rect;                         // 文本bound
  float x = 0.0;                            // 渲染基线 x
  float y = 0.0;                            // 渲染基线 y
  SkFont font = font::default_font;       // 字体
  std::string text{};                       // label
  SkPaint paint = PaintDesc{};            // paint
  sk_sp<SkTextBlob> blob;                   // 文本blob
  Alignment alignment = Alignment::TopLeft; // 对齐方式

public:

  /**
   * 构造一个文本节点
   * @param text 显示的文本
   * @param rect 所占空间
   */
  RenderText(std::string_view text, const SkRect& rect);
  RenderText() = default;
  void resize(float width, float height) noexcept override;

  /**
   * 设置对齐方式
   */
  void setAlignment(Alignment alignment);

  /**
   * 设置字体大小
   * @param size 字体大小
   */
  void setFontSize(float size);

  /**
   * 设置字体颜色
   */
  void setColor(SkColor color);

  /**
   * 设置文本
   */
  void setText(std::string_view text);

  /**
   * render
   */
  void render(SkCanvas *canvas) override;

private:
  /**
   * 重新计算文本、位置等信息
   */
  void reCalculate();
};

RenderText::RenderText(const std::string_view text, const SkRect& rect): RenderNode(rect), text(text) {
  reCalculate();
}

void RenderText::resize(float width, float height) noexcept {
  RenderNode::resize(width, height);
  reCalculate();
}

void RenderText::setAlignment(const Alignment alignment) {
  this->alignment = alignment;
  reCalculate();
}

void RenderText::setFontSize(const float size) {
  font.setSize(size);
  reCalculate();
}

void RenderText::setColor(const SkColor color) {
  paint.setColor(color);
}

void RenderText::setText(const std::string_view text) {
  this->text = text;
  reCalculate();
}

void RenderText::render(SkCanvas *canvas) {
  if (visible) {
    canvas->drawTextBlob(blob, x, y, paint);
  }
}

void RenderText::reCalculate() {
  // 计算bound
  font.measureText(this->text.data(), this->text.size(), SkTextEncoding::kUTF8, &font_rect);

  // 计算起点x
  if (alignment & Alignment::Left) {
    x = local_rect_.left() - font_rect.left();
  } else if (alignment & Alignment::Right) {
    x = local_rect_.right() - font_rect.right();
  } else if (alignment & Alignment::HCenter) {
    x = local_rect_.centerX() - font_rect.centerX();
  }

  // 计算起点y
  if (alignment & Alignment::Top) {
    y = local_rect_.top() - font_rect.top();
  } else if (alignment & Alignment::Bottom) {
    y = local_rect_.bottom() - font_rect.bottom();
  } else if (alignment & Alignment::VCenter) {
    y = local_rect_.centerY() - font_rect.centerY();
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
