module;
// ReSharper disable once CppUnusedIncludeDirective
#include "include/private/base/SkFloatingPoint.h"
export module ui.render:text;

import :base;
import skia.resource;
import std;

using namespace skia;

namespace {

/**
 * 配置文本字体渲染参数
 * @param font 待配置的字体
 */
void configureTextFont(SkFont &font) noexcept;

/**
 * 将文本绘制坐标吸附到整数像素
 * @param value 原始坐标
 * @return 吸附后的坐标
 */
float snapTextPosition(float value) noexcept;

/**
 * 测量 UTF-8 文本宽度。
 * @param font 文本字体
 * @param text 待测量文本
 * @return 文本宽度
 */
float measureTextWidth(const SkFont &font, std::string_view text);

/**
 * 收集 UTF-8 字符前缀边界。
 * @param text 原始文本
 * @return 每个字符结束后的字节位置
 */
std::vector<std::size_t> utf8PrefixEnds(std::string_view text);

/**
 * 生成可放入指定宽度的省略号文本。
 * @param font 文本字体
 * @param max_width 最大宽度
 * @return 可显示的省略号文本
 */
std::string fitEllipsis(const SkFont &font, float max_width);

/**
 * 按最大宽度截断文本并追加省略号。
 * @param font 文本字体
 * @param text 原始文本
 * @param max_width 最大宽度
 * @return 截断后的文本
 */
std::string ellipsizeText(const SkFont &font, std::string_view text, float max_width);

} // namespace

namespace {

void configureTextFont(SkFont &font) noexcept {
  font.setEdging(SkFont::Edging::kSubpixelAntiAlias);
  font.setHinting(SkFontHinting::kFull);
  font.setSubpixel(false);
  font.setLinearMetrics(false);
}

float snapTextPosition(const float value) noexcept { return std::round(value); }

float measureTextWidth(const SkFont &font, const std::string_view text) {
  SkRect bounds{};
  font.measureText(text.data(), text.size(), SkTextEncoding::kUTF8, &bounds);
  return bounds.width();
}

std::vector<std::size_t> utf8PrefixEnds(const std::string_view text) {
  std::vector<std::size_t> ends;
  ends.reserve(text.size());
  for (std::size_t i = 0; i < text.size();) {
    const auto ch = static_cast<unsigned char>(text[i]);
    std::size_t char_size = 1;
    if ((ch & 0b1110'0000) == 0b1100'0000) {
      char_size = 2;
    } else if ((ch & 0b1111'0000) == 0b1110'0000) {
      char_size = 3;
    } else if ((ch & 0b1111'1000) == 0b1111'0000) {
      char_size = 4;
    }
    i = std::min(text.size(), i + char_size);
    ends.push_back(i);
  }
  return ends;
}

std::string fitEllipsis(const SkFont &font, const float max_width) {
  constexpr std::string_view ellipsis = "...";
  for (std::size_t len = ellipsis.size(); len > 0; --len) {
    if (measureTextWidth(font, ellipsis.substr(0, len)) <= max_width) {
      return std::string(ellipsis.substr(0, len));
    }
  }
  return {};
}

std::string ellipsizeText(const SkFont &font, const std::string_view text, const float max_width) {
  if (text.empty() || max_width <= 0.0f || measureTextWidth(font, text) <= max_width) {
    return std::string(text);
  }

  const auto ellipsis = fitEllipsis(font, max_width);
  const float ellipsis_width = measureTextWidth(font, ellipsis);
  if (ellipsis.empty() || ellipsis_width >= max_width) {
    return ellipsis;
  }

  const auto ends = utf8PrefixEnds(text);
  std::size_t lo = 0;
  std::size_t hi = ends.size();
  while (lo < hi) {
    const std::size_t mid = (lo + hi + 1) / 2;
    const auto prefix = text.substr(0, ends[mid - 1]);
    if (measureTextWidth(font, prefix) + ellipsis_width <= max_width) {
      lo = mid;
    } else {
      hi = mid - 1;
    }
  }

  if (lo == 0) {
    return ellipsis;
  }
  std::string result(text.substr(0, ends[lo - 1]));
  result.append(ellipsis);
  return result;
}

} // namespace

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
  RenderText();

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

RenderText::RenderText(const std::string_view text, const SkRect &rect)
    : RenderNode(rect), text_(text) {
  configureTextFont(font);
  RenderText::update();
}

RenderText::RenderText(const std::string_view text) : text_(text) { configureTextFont(font); }

RenderText::RenderText() { configureTextFont(font); }

const SkRect &RenderText::textBound() const { return font_rect; }

void RenderText::setFontSize(const float size) {
  font.setSize(size);
  configureTextFont(font);
  update();
}

void RenderText::setFont(const SkFont &f) {
  font = f;
  configureTextFont(font);
  update();
}

void RenderText::setColor(const SkColor color) { paint.setColor(color); }

void RenderText::setText(const std::string_view text) {
  this->text_ = text;
  update();
}

void RenderText::setTextAndAlignment(const std::string_view text, const Alignment alignment) {
  this->text_ = text;
  alignment_ = alignment;
  update();
}

const std::string &RenderText::text() const { return text_; }

void RenderText::render(SkCanvas *canvas) {
  if (visible) {
    canvas->drawTextBlob(blob, x, y, paint);
  }
}

void RenderText::update() {
  const auto display_text = ellipsizeText(font, text_, std::max(0.0f, self_box.width()));
  font.measureText(display_text.data(), display_text.size(), SkTextEncoding::kUTF8, &font_rect);
  const auto pos = calcAlignedPosition(font_rect);
  x = snapTextPosition(pos.x());
  y = snapTextPosition(pos.y());

  blob = SkTextBlob::MakeFromText(display_text.data(), display_text.size(), font,
                                  SkTextEncoding::kUTF8);
}

} // namespace ui::render
