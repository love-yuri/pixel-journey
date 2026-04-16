//
// Created by love-yuri on 2026/1/29.
//
export module ui.widgets:splitter;

import std;
import skia;
import ui.layout;
import :base;

using namespace ui::layout;
using namespace skia;

export namespace ui::widgets {

enum class SplitterOrientation {
  Horizontal,
  Vertical,
};

class Splitter;

class SplitterLayout : public Layout<Widget> {
public:
  explicit SplitterLayout(Widget *widget) : Layout(widget) {
  }
  void apply() const override;
};

class Splitter : public Widget {
  friend class SplitterLayout;

public:
  explicit Splitter(Widget *parent) :
    Widget(parent), paint_(SkPaintBuilder().setColor(skia_colors::gray).build()) {
    setLayout<SplitterLayout>();
  }

  // 设置分割位置
  void setSplitPosition(const float pos) {
    split_pos = std::clamp(pos, effectiveMinLeft(), effectiveMaxLeft());
    markLayoutDirty();
  }

  // 设置左侧面板最小宽度
  void setMinLeftWidth(const float w) {
    min_left_width = w;
  }

  // 设置左侧面板最大宽度
  void setMaxLeftWidth(const float w) {
    max_left_width = w;
  }

  // split的位置
  [[nodiscard]] float splitPosition() const {
    return split_pos;
  }

  // 左侧最小宽度
  [[nodiscard]] float minLeftWidth() const {
    return min_left_width;
  }

  // 左侧最大宽度
  [[nodiscard]] float maxLeftWidth() const {
    return max_left_width;
  }

  void addWidget(Widget *widget) override {
    if (children_.size() >= 2) return;
    Widget::addWidget(widget);
    markLayoutDirty();
  }

  void layoutChildren() override {
    if (layout_) {
      layout_->apply();
    }
    for (const auto child : children_) {
      child->updateLayout();
    }
  }

protected:
  void onMouseMove(const float x, const float y) override {
    if (dragging_) {
      split_pos = std::clamp(x, effectiveMinLeft(), effectiveMaxLeft());
      markLayoutDirty();
    } else {
      const bool near = std::abs(x - split_pos) <= handle_w / 2.f + hit_zone;
      handle_w = near ? active_handle_w : default_handle_w;
      if (near) {
        window()->setCursor(CursorType::HResize);
      }
    }
  }

  void onMouseLeftPressed(const float x, const float y) override {
    if (std::abs(x - split_pos) <= handle_w / 2.f + hit_zone) {
      dragging_ = true;
      is_dragging = true;
      handle_w = active_handle_w;
      window()->setCursor(CursorType::HResize);
    }
  }

  void onMouseLeftReleased(float, float) override {
    if (dragging_) {
      window()->setCursor(CursorType::Arrow);
      dragging_ = false;
      is_dragging = false;
    }
    handle_w = default_handle_w;
  }

public:
  void paint(SkCanvas *canvas) override {
    if (children_.size() < 2) return;
    canvas->drawRect(SkRect::MakeXYWH(split_pos - handle_w / 2.f, 0, handle_w, contentHeight()),
                     paint_);
  }

private:
  // 计算有效的左侧最小宽度
  [[nodiscard]] float effectiveMinLeft() const {
    return min_left_width > 0.f ? min_left_width : 0.f;
  }
  // 计算有效的左侧最大宽度
  [[nodiscard]] float effectiveMaxLeft() const {
    return max_left_width > 0.f ? std::min(max_left_width, contentWidth()) : contentWidth();
  }

  float split_pos = 250.f;    // 分割位置
  float min_left_width = 0.f; // 左侧面板最小宽度，0 表示无限制
  float max_left_width = 0.f; // 左侧面板最大宽度，0 表示无限制
  static constexpr float default_handle_w = 1.f;
  static constexpr float active_handle_w = 5.f;
  static constexpr float hit_zone = 3.f;
  float handle_w = default_handle_w;
  bool dragging_ = false;
  SkPaint paint_;
};

void SplitterLayout::apply() const {
  const auto &children = widget_->children();
  const auto n = children.size();
  if (n == 0) return;

  const auto w = widget_->contentWidth();
  const auto h = widget_->contentHeight();
  const auto *splitter = static_cast<const Splitter *>(widget_);
  const auto split = splitter->splitPosition();

  if (n == 1) {
    setGeometry(children[0], 0, 0, w, h);
  } else {
    setGeometry(children[0], 0, 0, split, h);
    setGeometry(children[1], split, 0, w - split, h);
  }
}

} // namespace ui::widgets
