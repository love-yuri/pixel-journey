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
  explicit SplitterLayout(Widget *widget) : Layout(widget) {}
  void apply() const override;
};

class Splitter : public Widget {
  friend class SplitterLayout;

public:
  explicit Splitter(Widget *parent)
      : Widget(parent), paint_(SkPaintBuilder().setColor(skia_colors::gray).build()) {
    setLayout<SplitterLayout>();
  }

  void setSplitPosition(const float pos) {
    split_pos_ = std::clamp(pos, 0.f, contentWidth());
    markLayoutDirty();
  }

  void setHandleWidth(const float w) { handle_w_ = w; }

  [[nodiscard]] float splitPosition() const { return split_pos_; }

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
      split_pos_ = std::clamp(x, 0.f, contentWidth());
      markLayoutDirty();
    } else {
      const bool near = std::abs(x - split_pos_) <= handle_w_ / 2.f + hit_zone_;
      handle_w_ = near ? active_handle_w_ : default_handle_w_;
      window()->setCursor(near ? CursorType::HResize : CursorType::Arrow);
    }
  }

  void onMouseLeftPressed(const float x, const float y) override {
    if (std::abs(x - split_pos_) <= handle_w_ / 2.f + hit_zone_) {
      dragging_ = true;
      is_dragging = true;
      handle_w_ = active_handle_w_;
      window()->setCursor(CursorType::HResize);
    }
  }

  void onMouseLeftReleased(float, float) override {
    dragging_ = false;
    is_dragging = false;
    handle_w_ = default_handle_w_;
    window()->setCursor(CursorType::Arrow);
  }

public:
  void paint(SkCanvas *canvas) override {
    if (children_.size() < 2) return;
    canvas->drawRect(SkRect::MakeXYWH(split_pos_ - handle_w_ / 2.f, 0, handle_w_, contentHeight()), paint_);
  }

private:
  float split_pos_ = 250.f;
  static constexpr float default_handle_w_ = 1.f;
  static constexpr float active_handle_w_ = 5.f;
  static constexpr float hit_zone_ = 3.f;
  float handle_w_ = default_handle_w_;
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
