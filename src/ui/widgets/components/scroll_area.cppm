//
// Created by yuri on 2026/4/29.
//

export module ui.widgets:scroll_area;

import :base;
import core;
import skia.api;
import std;

using namespace skia;

export namespace ui::widgets {

/**
 * 滚动区域控件
 * 将子控件垂直堆叠，超出可见区域时支持鼠标滚轮滚动并显示滚动条
 * 子控件通过 setMaxHeight/setMinHeight 设置自身高度
 */
class ScrollArea : public Widget {


public:
  explicit ScrollArea(Widget *parent = nullptr);
  void paint(SkCanvas *canvas) override;
  void render(SkCanvas *canvas) override;
  void layoutChildren() override;

  // 滚动位置变化信号（参数为当前滚动偏移量）
  Signal<float> scrollChanged;

  // 获取当前滚动偏移量
  [[nodiscard]] float scrollOffset() const noexcept { return scroll_offset; }

  // 获取最大滚动偏移量
  [[nodiscard]] float maxScroll() const noexcept {
    return std::max(0.f, content_height_ - contentHeight());
  }

protected:
  void MouseMove(float x, float y) override;
  void MouseLeftPressed(float x, float y) override;
  void MouseLeftReleased(float x, float y) override;
  void MouseWheel(float x, float y, float delta_x, float delta_y) override;
  void onMouseWheel(float delta_x, float delta_y) override;

private:
  float content_height_ = 0;                // 内容总高度
  float scroll_offset = 0;                 // 当前滚动偏移量
  static constexpr float kBarWidth = 6;     // 滚动条宽度
  static constexpr float kBarPadding = 4;   // 滚动条边距
  static constexpr float kMinThumbH = 24;   // 滑块最小高度
  static constexpr float kScrollSpeed = 40; // 滚轮速度（像素/档）
};

ScrollArea::ScrollArea(Widget *parent) : Widget(parent) {
}

void ScrollArea::MouseMove(const float x, const float y) {
  Widget::MouseMove(x, y + scroll_offset);
}

void ScrollArea::MouseLeftPressed(const float x, const float y) {
  Widget::MouseLeftPressed(x, y + scroll_offset);
}

void ScrollArea::MouseLeftReleased(const float x, const float y) {
  Widget::MouseLeftReleased(x, y + scroll_offset);
}

void ScrollArea::MouseWheel(const float x, const float y, const float delta_x, const float delta_y) {
  Widget::MouseWheel(x, y + scroll_offset, delta_x, delta_y);
}

void ScrollArea::onMouseWheel(const float delta_x, float const delta_y) {
  scroll_offset -= delta_y * kScrollSpeed;

  const float view_h = contentHeight();
  const float max_scroll = std::max(0.f, content_height_ - view_h);
  scroll_offset = std::clamp(scroll_offset, 0.f, max_scroll);

  scrollChanged.emit(scroll_offset);
}

void ScrollArea::paint(SkCanvas *canvas) {
  const float view_h = contentHeight();
  if (content_height_ <= view_h) return; // 内容未超出，不绘制滚动条

  const float bar_x = contentWidth() - kBarPadding - kBarWidth;
  const float thumb_h = std::max(kMinThumbH, view_h * view_h / content_height_);
  const float max_scroll = content_height_ - view_h;
  const float scroll_pct = max_scroll > 0 ? scroll_offset / max_scroll : 0;
  const float thumb_y = scroll_pct * (view_h - thumb_h);

  SkPaint track_paint;
  track_paint.setColor(ColorFromARGB(40, 255, 255, 255));
  track_paint.setAntiAlias(true);
  canvas->drawRoundRect(SkRect::MakeXYWH(bar_x, 0, kBarWidth, view_h), kBarWidth / 2, kBarWidth / 2,
                        track_paint);

  SkPaint thumb_paint;
  thumb_paint.setColor(ColorFromARGB(120, 255, 255, 255));
  thumb_paint.setAntiAlias(true);
  canvas->drawRoundRect(SkRect::MakeXYWH(bar_x, thumb_y, kBarWidth, thumb_h), kBarWidth / 2,
                        kBarWidth / 2, thumb_paint);
}

void ScrollArea::render(SkCanvas *canvas) {
  const float view_w = contentWidth();
  const float view_h = contentHeight();

  canvas->save();
  canvas->translate(x_, y_);

  // 裁剪可见区域
  canvas->translate(padding_.left, padding_.top);
  canvas->clipRect(SkRect::MakeWH(view_w, view_h));

  // 应用滚动偏移
  canvas->translate(0, -scroll_offset);

  // 仅绘制可见子控件（y 区间与视口有交集）
  for (const auto child : children_) {
    if (!child->visible()) {
      continue;
    }
    const float cy = child->y();
    const float ch = child->height();
    if (cy + ch <= scroll_offset || cy >= scroll_offset + view_h) {
      continue;
    } // 完全不可见
    child->render(canvas);
  }

  canvas->restore();

  // 滚动条绘制在裁剪区域之外（不受 scroll offset 影响）
  canvas->save();
  canvas->translate(x_ + padding_.left, y_ + padding_.top);
  paint(canvas);
  canvas->restore();
}

void ScrollArea::layoutChildren() {
  float y = 0;
  const float w = contentWidth();

  for (const auto child : children_) {
    if (!child->visible()) continue;

    const auto &sc = child->sizeConstraints();
    const float child_w = sc.clampW(w);
    const float child_h = std::isfinite(sc.max_h) ? sc.max_h : sc.clampH(50);

    child->setGeometry(0, y, child_w, child_h);
    y += child_h;
  }

  // 记录内容总高度并重新钳制滚动偏移（可能因窗口 resize 而变化）
  content_height_ = y;
  const float max_scroll = std::max(0.f, content_height_ - contentHeight());
  scroll_offset = std::clamp(scroll_offset, 0.f, max_scroll);

  scrollChanged.emit(scroll_offset);

  // 递归更新子控件布局
  for (const auto child : children_) {
    child->updateLayout();
  }
}

} // namespace ui::widgets
