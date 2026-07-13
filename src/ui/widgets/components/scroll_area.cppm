//
// Created by yuri on 2026/4/29.
//

export module ui.widgets:scroll_area;

import :base;
import core;
import skia.api;
import ui.animation;
import std;

using namespace skia;
using namespace ui::animation;
using namespace profiling;

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

  /** 分发滚动内容区域内的鼠标右键事件 */
  void MouseRightPressed(float x, float y) override;

  /** 分发滚动内容区域内的鼠标右键松开事件 */
  void MouseRightReleased(float x, float y) override;

  void MouseWheel(float x, float y, float delta_x, float delta_y) override;
  void onMouseMove(float x, float y) override;
  void onMouseLeftReleased(float x, float y) override;
  void onMouseWheel(float delta_x, float delta_y) override;

private:
  void setScrollOffset(float value);
  void setScrollbarFeedback(float value) noexcept { scrollbar_t_ = value; }

  /** 平滑激活滚动条（变大变亮），并重置淡出计时 */
  void activateScrollbar() noexcept;

  /** 每帧检查并调度延迟淡出 */
  void scheduleScrollbarFadeOut() noexcept;

  float content_height_ = 0;                 // 内容总高度
  float scroll_offset = 0;                   // 当前滚动偏移量
  float target_scroll_offset_ = 0;           // 滚轮动画目标偏移量
  float scrollbar_t_ = 0;                    // 滚动条高亮强度
  bool thumb_dragging_ = false;              // 是否正在拖动滑块
  bool fading_out_ = false;                  // 滚动条是否正在淡出
  std::uint64_t last_interaction_time_ = 0;  // 最后一次交互时间（微秒）
  float thumb_drag_start_y_ = 0;             // 拖动起始鼠标 y（视口坐标）
  float drag_start_scroll_ = 0;              // 拖动起始 scroll_offset
  static constexpr float kBarWidth = 4;      // 滚动条宽度（激活前后一致）
  static constexpr float kBarPadding = 4;    // 滚动条边距
  static constexpr float kMinThumbH = 24;    // 滑块最小高度
  static constexpr float kScrollSpeed = 40;  // 滚轮速度（像素/档）
  static constexpr float kSmoothScrollMs = 180.0f;  // 滚动平滑时长
  static constexpr float kActivateMs = 160.0f;      // 滚动条激活动画时长
  static constexpr float kFadeDelayMs = 360.0f;     // 停止交互后淡出延迟
  static constexpr float kFadeMs = 420.0f;          // 滚动条淡出时长
};

ScrollArea::ScrollArea(Widget *parent) : Widget(parent) {
}

void ScrollArea::MouseMove(const float x, const float y) {
  if (thumb_dragging_) {
    onMouseMove(x, y);
    return;
  }
  Widget::MouseMove(x, y + scroll_offset);
}

void ScrollArea::MouseLeftPressed(const float x, const float y) {
  // 视口坐标下检测是否命中滑块
  const float view_h = contentHeight();
  const float max_scroll = std::max(0.f, content_height_ - view_h);
  if (max_scroll > 0) {
    const float thumb_h = std::max(kMinThumbH, view_h * view_h / content_height_);
    const float scroll_pct = scroll_offset / max_scroll;
    const float thumb_y = scroll_pct * (view_h - thumb_h);
    const float bar_x = contentWidth() - kBarPadding - kBarWidth;
    if (y >= thumb_y && y <= thumb_y + thumb_h && (x - padding_.left) >= bar_x) {
      thumb_dragging_ = true;
      is_dragging = true;
      thumb_drag_start_y_ = y;
      drag_start_scroll_ = scroll_offset;
      // 拖拽即时显示滚动条，并重置淡出计时
      fading_out_ = false;
      last_interaction_time_ = frame_clock.now;
      scrollbar_t_ = 1.0f;
      return; // 不转发给子控件
    }
  }
  Widget::MouseLeftPressed(x, y + scroll_offset);
}

void ScrollArea::MouseLeftReleased(const float x, const float y) {
  Widget::MouseLeftReleased(x, y + scroll_offset);
}

void ScrollArea::MouseRightPressed(const float x, const float y) {
  Widget::MouseRightPressed(x, y + scroll_offset);
}

void ScrollArea::MouseRightReleased(const float x, const float y) {
  Widget::MouseRightReleased(x, y + scroll_offset);
}

void ScrollArea::MouseWheel(const float x, const float y, const float delta_x, const float delta_y) {
  Widget::MouseWheel(x, y + scroll_offset, delta_x, delta_y);
}

void ScrollArea::onMouseWheel(const float delta_x, float const delta_y) {
  const float view_h = contentHeight();
  const float max_scroll = std::max(0.f, content_height_ - view_h);
  const float next = std::clamp(target_scroll_offset_ - delta_y * kScrollSpeed, 0.f, max_scroll);
  if (std::abs(next - target_scroll_offset_) <= 0.01f) {
    return;
  }

  target_scroll_offset_ = next;
  startAnimation<&ScrollArea::setScrollOffset>(
    scroll_offset,
    target_scroll_offset_,
    kSmoothScrollMs,
    CubicBezier(0.22f, 0.9f, 0.22f, 1.0f)
  );

  activateScrollbar();
}

void ScrollArea::onMouseMove(const float x, float y) {
  if (!thumb_dragging_) return;
  const float view_h = contentHeight();
  const float thumb_h = std::max(kMinThumbH, view_h * view_h / content_height_);
  const float max_scroll = std::max(0.f, content_height_ - view_h);
  const float dy = y - thumb_drag_start_y_;
  setScrollOffset(drag_start_scroll_ + dy * max_scroll / (view_h - thumb_h));
  target_scroll_offset_ = scroll_offset;
  // 拖拽进行中，保持滚动条高亮并重置淡出计时
  fading_out_ = false;
  last_interaction_time_ = frame_clock.now;
}

void ScrollArea::onMouseLeftReleased(float, float) {
  if (thumb_dragging_) {
    thumb_dragging_ = false;
    is_dragging = false;
    // 拖拽结束，进入延迟淡出阶段
    fading_out_ = false;
    last_interaction_time_ = frame_clock.now;
  }
}

void ScrollArea::activateScrollbar() noexcept {
  // 取消正在进行的淡出
  fading_out_ = false;
  // 记录最后一次交互时刻，用于延迟淡出计时
  last_interaction_time_ = frame_clock.now;

  // 平滑过渡高亮强度到 1；无条件启动可借助去重机制取消进行中的淡出动画
  startAnimation<&ScrollArea::setScrollbarFeedback>(
    scrollbar_t_, 1.0f, kActivateMs, CubicBezier::EaseOut());
}

void ScrollArea::scheduleScrollbarFadeOut() noexcept {
  // 拖拽中、已不可滚动或滚动条本就隐藏时不参与淡出调度
  if (thumb_dragging_ || content_height_ <= contentHeight() || scrollbar_t_ <= 0.001f) {
    return;
  }

  // 已在淡出，等待动画自行结束
  if (fading_out_) {
    return;
  }

  // 距离最后一次交互超过延迟阈值，启动淡出
  const auto elapsed_us = frame_clock.now - last_interaction_time_;
  if (elapsed_us >= static_cast<std::uint64_t>(kFadeDelayMs * 1000.0f)) {
    fading_out_ = true;
    startAnimation<&ScrollArea::setScrollbarFeedback>(
      scrollbar_t_, 0.0f, kFadeMs, CubicBezier::EaseInOut());
  }
}

void ScrollArea::paint(SkCanvas *canvas) {
  const float view_h = contentHeight();
  if (content_height_ <= view_h) return; // 内容未超出，不绘制滚动条

  const float thumb_h = std::max(kMinThumbH, view_h * view_h / content_height_);
  const float max_scroll = content_height_ - view_h;
  const float scroll_pct = max_scroll > 0 ? scroll_offset / max_scroll : 0;
  const float thumb_y = scroll_pct * (view_h - thumb_h);
  // 拖拽时强制满强度，其余时刻跟随激活动画值
  const float active_t = std::max(scrollbar_t_, thumb_dragging_ ? 1.0f : 0.0f);
  // 激活前后宽度保持一致，避免变形突兀
  const float bar_w = kBarWidth;
  const float active_bar_x = contentWidth() - kBarPadding - bar_w;

  SkPaint track_paint;
  track_paint.setColor(ColorFromARGB(static_cast<U8CPU>(30.0f + active_t * 28.0f), 128, 128, 128));
  track_paint.setAntiAlias(true);
  canvas->drawRoundRect(SkRect::MakeXYWH(active_bar_x, 0, bar_w, view_h), bar_w / 2, bar_w / 2,
                        track_paint);

  SkPaint thumb_paint;
  thumb_paint.setColor(ColorFromARGB(static_cast<U8CPU>(112.0f + active_t * 76.0f), 128, 128, 128));
  thumb_paint.setAntiAlias(true);
  canvas->drawRoundRect(SkRect::MakeXYWH(active_bar_x, thumb_y, bar_w, thumb_h), bar_w / 2,
                        bar_w / 2, thumb_paint);
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

  // 每帧检查是否需要启动延迟淡出
  scheduleScrollbarFadeOut();

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
  target_scroll_offset_ = std::clamp(target_scroll_offset_, 0.f, max_scroll);

  scrollChanged.emit(scroll_offset);

  // 递归更新子控件布局
  for (const auto child : children_) {
    child->updateLayout();
  }
}

void ScrollArea::setScrollOffset(const float value) {
  const float max_scroll = std::max(0.f, content_height_ - contentHeight());
  const float next = std::clamp(value, 0.f, max_scroll);
  if (std::abs(scroll_offset - next) <= 0.01f) {
    return;
  }
  scroll_offset = next;
  scrollChanged.emit(scroll_offset);
}

} // namespace ui::widgets
