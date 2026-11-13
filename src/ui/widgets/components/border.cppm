export module yuri.ui.widgets:border;

import std;
import :base;
import yuri.ui.layout.core;
import yuri.ui.render;
import yuri.skia.resource;

using namespace skia;
using namespace ui::layout;
using namespace ui::render;

namespace {

/** 在 Border 的内边距区域中排列唯一的子控件。 */
class BorderLayout final : public Layout<ui::widgets::Widget> {
public:
  /**
   * 根据内边距、子控件外边距、尺寸约束和对齐方式提交布局结果。
   * @param parent 拥有此布局的 Border 控件
   */
  void apply(ui::widgets::Widget &parent) const override;
};

} // namespace

export namespace ui::widgets {

/** 提供背景、统一圆角和内边距的单子控件容器。 */
class Border : public Widget {
public:
  /**
   * 创建背景透明、圆角和内边距均为零的容器。
   * @param parent 父控件
   */
  explicit Border(Widget *parent = nullptr);

  /** 获取背景颜色。 */
  [[nodiscard]] SkColor bg() const noexcept {
    return render_bg_.color();
  }

  /**
   * 设置背景颜色。
   * @param color 背景颜色，支持透明度
   */
  void set_background(SkColor color) noexcept;

  /** 获取统一的圆角半径。 */
  [[nodiscard]] float radius() const noexcept {
    return radius_;
  }

  /**
   * 设置背景的圆角半径，不裁剪子控件。
   * @param radius 圆角半径，负值按零处理
   */
  void set_radius(float radius) noexcept;

  /** 获取四边内边距。 */
  [[nodiscard]] Insets padding() const noexcept {
    return padding_;
  }

  /**
   * 设置四边相同的内边距，并触发重新布局。
   * @param padding 四边内边距，负值按零处理
   */
  void set_padding(float padding) noexcept;

  /**
   * 设置各边内边距，并触发重新布局。
   * @param padding 各边内边距，负值按零处理
   */
  void set_padding(const Insets &padding) noexcept;

  /** 获取唯一的子控件，没有内容时返回空指针。 */
  [[nodiscard]] Widget *child() const noexcept {
    return children_.empty() ? nullptr : children_.front();
  }

  /** 获取扣除内边距后的本地内容区域，宽高最小为零。 */
  [[nodiscard]] SkRect content_bounds() const noexcept;

  /**
   * 添加唯一的子控件，沿用 Widget 的子控件所有权规则。
   * @param widget 子控件，空指针不执行操作
   * @throws std::logic_error 已有子控件时拒绝添加第二个子控件
   */
  void add_widget(Widget *widget) override;

  /**
   * 绘制背景，子控件由 Widget 按布局后的坐标绘制。
   * @param canvas 目标画布
   */
  void paint(SkCanvas *canvas) override;

  /** 更新背景区域，并通过内部布局排列和更新子控件。 */
  void layout_children() override;

private:
  SkColor bg_ = skia_colors::transparent; // 背景颜色
  float radius_ = 0.0f;                   // 统一圆角半径
  Insets padding_;                        // 四边内边距
  RenderBackground render_bg_{};          // 背景绘制节点
};

Border::Border(Widget *parent) : Widget(parent) {
  render_bg_.radius = &radius_;
  set_layout(new BorderLayout());
}

void Border::set_background(const SkColor color) noexcept {
  bg_ = color;
  render_bg_.set_color(color);
}

void Border::set_radius(const float radius) noexcept {
  radius_ = std::max(0.0f, radius);
}

void Border::set_padding(const float padding) noexcept {
  set_padding(Insets(padding));
}

void Border::set_padding(const Insets &padding) noexcept {
  padding_ = Insets(
    std::max(0.0f, padding.left),
    std::max(0.0f, padding.top),
    std::max(0.0f, padding.right),
    std::max(0.0f, padding.bottom)
  );
  mark_layout_dirty();
}

SkRect Border::content_bounds() const noexcept {
  return SkRect::MakeXYWH(
    padding_.left,
    padding_.top,
    std::max(0.0f, width_ - padding_.left - padding_.right),
    std::max(0.0f, height_ - padding_.top - padding_.bottom)
  );
}

void Border::add_widget(Widget *widget) {
  if (widget == nullptr) {
    return;
  }

  if (!children_.empty()) {
    throw std::logic_error("Border 只能包含一个子控件，请使用带布局的 Widget 容纳多个控件。");
  }

  Widget::add_widget(widget);
  mark_layout_dirty(LayoutDirty::Children);
}

void Border::paint(SkCanvas *canvas) {
  render_bg_.draw(canvas);
}

void Border::layout_children() {
  render_bg_.update(local_bounds());
  Widget::layout_children();
}

} // namespace ui::widgets

namespace {

void BorderLayout::apply(ui::widgets::Widget &parent) const {
  const auto &border = static_cast<const ui::widgets::Border &>(parent);
  auto *const child = border.child();
  if (child == nullptr) return;

  const auto area = border.content_bounds();
  const auto margin = child->margin();
  const auto &constraints = child->size_constraints();
  const auto width = constraints.clampW(std::max(0.0f, area.width() - margin.left - margin.right));
  const auto height =
    constraints.clampH(std::max(0.0f, area.height() - margin.top - margin.bottom));
  const auto slot_width = width + margin.left + margin.right;
  const auto slot_height = height + margin.top + margin.bottom;
  const auto x =
    area.x() + calc_aligned_offset(child->horizontal_alignment(), area.width(), slot_width);
  const auto y =
    area.y() + calc_aligned_offset(child->vertical_alignment(), area.height(), slot_height);

  // padding 只在布局时扣除；arrange 再扣除 margin，绘制和命中检测共用最终坐标。
  arrange(*child, SkRect::MakeXYWH(x, y, slot_width, slot_height));
}

} // namespace
