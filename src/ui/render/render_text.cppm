module;
// ReSharper disable once CppUnusedIncludeDirective
#include "include/private/SkFloatingPoint.h"
export module yuri.ui.render:text;

import :base;
import yuri.skia.resource;
import std;

using namespace skia;

namespace {

/**
 * 测量 UTF-8 文本的可见边界宽度。
 * @param font 用于测量的字体
 * @param text 待测量文本
 * @return 文本的可见边界宽度
 */
float measure_text_width(const SkFont &font, std::string_view text);

/**
 * 收集 UTF-8 字符前缀边界。
 * @param text 原始文本
 * @return 每个字符结束后的字节位置
 */
std::vector<std::size_t> utf8_prefix_ends(std::string_view text);

/**
 * 生成可放入指定宽度的省略号文本。
 * @param font 用于测量的字体
 * @param max_width 最大可用宽度
 * @return 可显示的省略号文本，空间不足时为空
 */
std::string fit_ellipsis(const SkFont &font, float max_width);

/**
 * 按最大宽度截断文本，并在完整字符后追加省略号。
 * @param font 用于测量的字体
 * @param text 原始文本
 * @param max_width 最大可用宽度
 * @return 处理后的显示文本
 */
std::string ellipsize_text(const SkFont &font, std::string_view text, float max_width);

} // namespace

namespace {

float measure_text_width(const SkFont &font, const std::string_view text) {
  SkRect bounds{};
  font.measureText(text.data(), text.size(), SkTextEncoding::kUTF8, &bounds);
  return bounds.width();
}

std::vector<std::size_t> utf8_prefix_ends(const std::string_view text) {
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
    // 仅在完整字符边界上记录截断位置。
    i = std::min(text.size(), i + char_size);
    ends.push_back(i);
  }
  return ends;
}

std::string fit_ellipsis(const SkFont &font, const float max_width) {
  constexpr std::string_view kEllipsis = "..."; // 超宽文本使用的省略号
  for (std::size_t len = kEllipsis.size(); len > 0; --len) {
    if (measure_text_width(font, kEllipsis.substr(0, len)) <= max_width) {
      return std::string(kEllipsis.substr(0, len));
    }
  }
  return {};
}

std::string ellipsize_text(const SkFont &font,
                           const std::string_view text,
                           const float max_width) {
  if (text.empty() || max_width <= 0.0f || measure_text_width(font, text) <= max_width) {
    return std::string(text);
  }

  const auto ellipsis = fit_ellipsis(font, max_width);
  const float ellipsis_width = measure_text_width(font, ellipsis);
  if (ellipsis.empty() || ellipsis_width >= max_width) {
    return ellipsis;
  }

  const auto ends = utf8_prefix_ends(text);
  std::size_t lo = 0;
  std::size_t hi = ends.size();
  // 二分查找能够与省略号一起放入目标宽度的最长字符前缀。
  while (lo < hi) {
    const std::size_t mid = (lo + hi + 1) / 2;
    const auto prefix = text.substr(0, ends[mid - 1]);
    if (measure_text_width(font, prefix) + ellipsis_width <= max_width) {
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

/** 使用字形缓存绘制单行文本的节点。 */
class RenderText final : public RenderNode {
  using RenderNode::RenderNode;

  float x_ = 0.0;                            // 文字绘制原点的 x 坐标
  float y_ = 0.0;                            // 文字绘制原点的 y 坐标
  SkFont font_ = SkFont{font::default_font}; // 使用的字体
  std::string text_;                         // 原始文本
  SkPaint paint_ = PaintDesc{};              // 绘制样式
  sk_sp<SkTextBlob> blob_{};                 // 字形缓存
  Alignment alignment_{};                    // 内容对齐方式

public:
  /**
   * 创建文字节点，并设置默认字体渲染参数。
   * @param text 原始文本
   * @param rect 绘制区域
   */
  explicit RenderText(std::string_view text = "", SkRect rect = SkRect::MakeEmpty());

  /**
   * 测量当前显示文本的本地可见边界。
   * @return 根据绘制区域截断后的文本边界
   */
  [[nodiscard]] SkRect text_bound() const;

  /**
   * 获取原始文本。
   * @return 原始文本的只读引用
   */
  [[nodiscard]] const std::string &text() const;

  /**
   * 设置字号，并更新绘制缓存。
   * @param size 字号
   */
  void set_font_size(float size);

  /**
   * 替换字体，并使用传入的字体配置更新绘制缓存。
   * @param font 新字体
   */
  void set_font(const SkFont &font);

  /**
   * 设置文字颜色。
   * @param color 文字颜色
   */
  void set_color(SkColor color);

  /**
   * 设置原始文本，并更新绘制缓存。
   * @param text 原始文本
   */
  void set_text(std::string_view text);

  /**
   * 设置内容对齐方式，并更新绘制缓存。
   * @param alignment 水平与垂直对齐方式
   */
  void set_alignment(Alignment alignment);

  /**
   * 设置内容和对齐方式，并更新绘制缓存。
   * @param alignment 水平与垂直对齐方式
   * @param text 原始文本
   */
  void set_text_and_alignment(std::string_view text, Alignment alignment);

  /**
   * 使用已有字形缓存绘制文字。
   * @param canvas 目标画布
   */
  void draw(SkCanvas *canvas) override;

  /**
   * 更新绘制区域，重新计算显示文本、绘制原点和字形缓存。
   * @param rect 新的绘制区域
   */
  void update(SkRect rect) override;
};

RenderText::RenderText(const std::string_view text, const SkRect rect)
    : RenderNode(rect), text_(text) {
  font_.setEdging(SkFont::Edging::kAntiAlias);
  font_.setHinting(SkFontHinting::kNone);
  font_.setSubpixel(true);
  font_.setLinearMetrics(true);
}

SkRect RenderText::text_bound() const {
  auto font_rect = SkRect::MakeEmpty();
  const auto display_text = ellipsize_text(font_, text_, std::max(0.0f, self_box.width()));
  font_.measureText(display_text.data(), display_text.size(), SkTextEncoding::kUTF8, &font_rect);
  return font_rect;
}

void RenderText::set_font_size(const float size) {
  if (size == font_.getSize()) {
    return;
  }

  font_.setSize(size);
  update(self_box);
}

void RenderText::set_font(const SkFont &font) {
  font_ = font;
  update(self_box);
}

void RenderText::set_color(const SkColor color) {
  paint_.setColor(color);
}

void RenderText::set_text(const std::string_view text) {
  if (this->text_ == text) {
    return;
  }

  this->text_ = text;
  update(self_box);
}

void RenderText::set_alignment(const Alignment alignment) {
  if (alignment_ == alignment) {
    return;
  }

  alignment_ = alignment;
  update(self_box);
}

void RenderText::set_text_and_alignment(const std::string_view text, const Alignment alignment) {
  if (alignment_ == alignment && text == text_) {
    return;
  }
  alignment_ = alignment;
  text_ = text;
  update(self_box);
}

const std::string &RenderText::text() const {
  return text_;
}

void RenderText::draw(SkCanvas *canvas) {
  if (visible) {
    canvas->drawTextBlob(blob_, x_, y_, paint_);
  }
}

void RenderText::update(const SkRect rect) {
  self_box = rect;
  auto font_rect = SkRect::MakeEmpty();
  const auto display_text = ellipsize_text(font_, text_, std::max(0.0f, self_box.width()));
  font_.measureText(display_text.data(), display_text.size(), SkTextEncoding::kUTF8, &font_rect);
  const auto pos = alignment_.calc_aligned_position(self_box, font_rect);
  x_ = pos.x();
  y_ = pos.y();

  blob_ = SkTextBlob::MakeFromText(
    display_text.data(), display_text.size(), font_, SkTextEncoding::kUTF8
  );
}

} // namespace ui::render
