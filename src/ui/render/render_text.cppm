module;
// ReSharper disable once CppUnusedIncludeDirective
#include "include/private/base/SkFloatingPoint.h"
export module ui.render:text;

import :base;
import skia.resource;
import std;

using namespace skia;

export namespace ui::render {

class RenderText : public RenderNode {
  using RenderNode::RenderNode;
  SkRect font_rect{};
  float x = 0.0;
  float y = 0.0;
  SkFont font = font::default_font;
  std::string text_{};
  SkPaint paint = PaintDesc{};
  sk_sp<SkTextBlob> blob;

public:
  using RenderNode::update;

  RenderText(std::string_view text, const SkRect &rect);
  explicit RenderText(std::string_view text);
  RenderText() = default;

  [[nodiscard]] const SkRect &textBound() const;
  void setFontSize(float size);
  void setFont(const SkFont &f);
  void setColor(SkColor color);
  void setText(std::string_view text);
  void setTextAndAlignment(std::string_view text, Alignment alignment);
  [[nodiscard]] const std::string &text() const;
  void render(SkCanvas *canvas) override;
  void update() override;
};

RenderText::RenderText(const std::string_view text, const SkRect &rect) :
  RenderNode(rect), text_(text) {
  RenderText::update();
}

RenderText::RenderText(const std::string_view text) : text_(text) {
}

const SkRect &RenderText::textBound() const {
  return font_rect;
}

void RenderText::setFontSize(const float size) {
  font.setSize(size);
  update();
}

void RenderText::setFont(const SkFont &f) {
  font = f;
  update();
}

void RenderText::setColor(const SkColor color) {
  paint.setColor(color);
}

void RenderText::setText(const std::string_view text) {
  this->text_ = text;
  update();
}

void RenderText::setTextAndAlignment(const std::string_view text, const Alignment alignment) {
  this->text_ = text;
  alignment_ = alignment;
  update();
}

const std::string &RenderText::text() const {
  return text_;
}

void RenderText::render(SkCanvas *canvas) {
  if (visible) {
    canvas->drawTextBlob(blob, x, y, paint);
  }
}

void RenderText::update() {
  font.measureText(text_.data(), text_.size(), SkTextEncoding::kUTF8, &font_rect);
  const auto pos = calcAlignedPosition(font_rect);
  x = pos.x();
  y = pos.y();

  blob = SkTextBlob::MakeFromText(text_.data(), text_.size(), font, SkTextEncoding::kUTF8);
}

} // namespace ui::render
