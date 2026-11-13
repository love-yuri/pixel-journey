//
// Created by love-yuri on 2026/1/29.
//
export module yuri.ui.widgets:splitter;

import std;
import yuri.skia;
import yuri.ui.layout.core;
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
  /**
   * 根据分割位置排列面板。
   * @param parent 拥有当前布局的分割控件
   */
  void apply(Widget &parent) const override;
};

class Splitter : public Widget {
public:
  explicit Splitter(Widget *parent) :
    Widget(parent), paint_(SkPaintBuilder().setColor(skia_colors::gray).build()) {
    set_layout<SplitterLayout>();
  }

  // 设置分割位置
  void set_split_position(const float pos) {
    split_position_ = std::clamp(pos, effective_min_left(), effective_max_left());
    mark_layout_dirty();
  }

  // 设置左侧面板最小宽度
  void set_min_left_width(const float w) {
    min_left_width_ = w;
    split_position_ = std::max(split_position_, w);
  }

  // 设置左侧面板最大宽度
  void set_max_left_width(const float w) {
    max_left_width_ = w;
    split_position_ = std::min(split_position_, w);
  }

  // split的位置
  [[nodiscard]] float split_position() const {
    return split_position_;
  }

  // 左侧最小宽度
  [[nodiscard]] float min_left_width() const {
    return min_left_width_;
  }

  // 左侧最大宽度
  [[nodiscard]] float max_left_width() const {
    return max_left_width_;
  }

  void add_widget(Widget *widget) override {
    if (children_.size() >= 2) return;
    Widget::add_widget(widget);
    mark_layout_dirty();
  }

  void layout_children() override {
    if (layout_) {
      layout_->apply(*this);
    }
    for (const auto child : children_) {
      child->update_layout();
    }
  }

protected:
  void on_mouse_move(const float x, const float y) override {
    if (dragging_) {
      split_position_ = std::clamp(x, effective_min_left(), effective_max_left());
      mark_layout_dirty();
    } else {
      const bool near = std::abs(x - split_position_) <= handle_width_ / 2.f + kHitZone;
      handle_width_ = near ? kActiveHandleWidth : kDefaultHandleWidth;
      if (near) {
        window()->setCursor(CursorType::HResize);
        has_near_set_ = true;
      } else if (has_near_set_) {
        window()->setCursor(CursorType::Arrow);
        has_near_set_ = false;
      }
    }
  }

  void on_mouse_left_pressed(const float x, const float y) override {
    if (std::abs(x - split_position_) <= handle_width_ / 2.f + kHitZone) {
      dragging_ = true;
      is_dragging_ = true;
      handle_width_ = kActiveHandleWidth;
      window()->setCursor(CursorType::HResize);
    }
  }

  void on_mouse_left_released(float, float) override {
    if (dragging_) {
      window()->setCursor(CursorType::Arrow);
      dragging_ = false;
      is_dragging_ = false;
    }
    handle_width_ = kDefaultHandleWidth;
  }

public:
  void paint(SkCanvas *canvas) override {
    if (children_.size() < 2) return;
    canvas->drawRect(SkRect::MakeXYWH(split_position_ - handle_width_ / 2.f, 0, handle_width_, height()),
                     paint_);
  }

private:
  // 计算有效的左侧最小宽度
  [[nodiscard]] float effective_min_left() const {
    return min_left_width_ > 0.f ? min_left_width_ : 0.f;
  }
  // 计算有效的左侧最大宽度
  [[nodiscard]] float effective_max_left() const {
    return max_left_width_ > 0.f ? std::min(max_left_width_, width()) : width();
  }

  float split_position_ = 220.f;                    // 分割位置
  float min_left_width_ = 0.f;                      // 左侧面板最小宽度，0 表示无限制
  float max_left_width_ = 0.f;                      // 左侧面板最大宽度，0 表示无限制
  static constexpr float kDefaultHandleWidth = 1.f; // 默认分割条宽度
  static constexpr float kActiveHandleWidth = 5.f;  // 激活后的分割条宽度
  static constexpr float kHitZone = 3.f;            // 分割条额外命中范围
  float handle_width_ = kDefaultHandleWidth;        // 当前分割条宽度
  bool dragging_ = false;                           // 是否正在拖动
  bool has_near_set_ = false;                       // 是否因靠近分割条而设置鼠标样式
  SkPaint paint_;                                   // 分割条画笔
};

void SplitterLayout::apply(Widget &parent) const {
  const auto &children = parent.children();
  const auto n = children.size();
  if (n == 0) return;

  const auto w = parent.width();
  const auto h = parent.height();
  const auto &splitter = static_cast<const Splitter &>(parent);
  const auto split = splitter.split_position();

  if (n == 1) {
    arrange(*children[0], SkRect::MakeXYWH(0.f, 0.f, w, h));
  } else {
    arrange(*children[0], SkRect::MakeXYWH(0.f, 0.f, split, h));
    arrange(*children[1], SkRect::MakeXYWH(split, 0.f, w - split, h));
  }
}

} // namespace ui::widgets
