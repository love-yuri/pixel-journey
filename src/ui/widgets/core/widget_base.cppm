//
// Created by love-yuri on 2026/1/16.
//
export module yuri.ui.widgets:base;

import :window_base;
import yuri.skia.api;
import yuri.ui.layout.core;
import yuri.ui.animation;
import yuri.core;
import std;

using namespace skia;
using namespace ui::layout;
using namespace ui::animation;

export namespace ui::widgets {

/**
 * 控件基类
 */
class Widget {
  WindowBase *window_ = nullptr;          // 窗口指针
  Widget *mouse_capture_ = nullptr;       // 正在被点击的控件
  Widget *right_mouse_capture_ = nullptr; // 正在被右键点击的控件
  friend class Layout<Widget>;            // 仅布局基类可提交布局结果

  /**
   * 应用布局结果，绕过手动几何修改限制，并在几何变化时标记布局失效。
   * @param rect 相对于父控件内容区域的几何信息
   */
  void apply_layout_geometry(const SkRect &rect) noexcept;

protected:
  Widget *parent_ = nullptr;                         // 父控件
  std::vector<Widget *> children_;                   // 子控件列表
  LayoutDirty layout_dirty_{};                       // 脏布局类型
  std::unique_ptr<Layout<Widget>> layout_ = nullptr; // 布局

  /* 控件几何信息 */
  float x_ = 0.f;                                              // 控件x 坐标
  float y_ = 0.f;                                              // 控件y 坐标
  float width_ = 0.f;                                          // 控件宽度
  float height_ = 0.f;                                         // 控件高度
  Insets margin_;                                              // 外边距，由父布局预留
  SizeConstraints size_constraints_;                           // 布局约束信息
  HorizontalAlignment horizontal_ = HorizontalAlignment::Left; // 水平对齐
  VerticalAlignment vertical_ = VerticalAlignment::Top;        // 垂直对齐

  /**
   * 是否展示 - 手动修改
   */
  std::uint8_t visible_ : 1 = 1;

  /**
   * 是否正在被hover - 自动修改
   * 判断鼠标是否在控件内，该属性自动修改，无需手动赋值
   */
  std::uint8_t hovered_ : 1 = 0;

  /**
   * 鼠标是否正在被拖拽 - 手动修改
   * 开启后move事件将不再检查是否在控件内
   */
  std::uint8_t is_dragging_ : 1 = 0;

  /**
   * 判断控件是否存在layout - 手动修改
   */
  std::uint8_t has_layout_ : 1 = 0;

  /**
   * 鼠标移动事件分发
   * @param x x: 相对于控件左上角
   * @param y y: 相对于控件左上角
   */
  virtual void mouse_move(float x, float y);

  /**
   * 鼠标左侧点击事件分发
   * @param x 鼠标位置x: 相对于控件左上角
   * @param y 鼠标位置y: 相对于控件左上角
   */
  virtual void mouse_left_pressed(float x, float y);

  /**
   * 鼠标左侧松开事件分发
   * @param x 鼠标位置x: 相对于控件左上角
   * @param y 鼠标位置y: 相对于控件左上角
   */
  virtual void mouse_left_released(float x, float y);

  /**
   * 鼠标右侧点击事件分发
   * @param x 鼠标位置x: 相对于控件左上角
   * @param y 鼠标位置y: 相对于控件左上角
   */
  virtual void mouse_right_pressed(float x, float y);

  /**
   * 鼠标右侧松开事件分发
   * @param x 鼠标位置x: 相对于控件左上角
   * @param y 鼠标位置y: 相对于控件左上角
   */
  virtual void mouse_right_released(float x, float y);

  /**
   * 鼠标滚轮事件分发
   * @param x 鼠标位置x（相对于控件左上角）
   * @param y 鼠标位置y（相对于控件左上角）
   * @param delta_x 水平滚动量
   * @param delta_y 垂直滚动量
   */
  virtual void mouse_wheel(float x, float y, float delta_x, float delta_y);

  /**
   * 鼠标移动事件
   * @param x 鼠标当前位置 x
   * @param y 相对于左上角
   */
  virtual void on_mouse_move(float x, float y) {
  }

  /**
   * 鼠标进入
   * @param x 鼠标当前位置 x
   * @param y 鼠标当前位置 y
   */
  virtual void on_mouse_enter(float x, float y) {
  }

  /**
   * 鼠标移出
   * @param x 鼠标当前位置 x
   * @param y 鼠标当前位置 y
   */
  virtual void on_mouse_leave(float x, float y) {
  }

  /**
   * 鼠标左侧点击事件
   * @param x 鼠标当前位置 x
   * @param y 鼠标当前位置 y
   */
  virtual void on_mouse_left_pressed(float x, float y) {
  }

  /**
   * 鼠标左侧松开事件
   * @param x 鼠标当前位置 x
   * @param y 鼠标当前位置 y
   */
  virtual void on_mouse_left_released(float x, float y) {
  }

  /**
   * 鼠标右侧点击事件
   * @param x 鼠标当前位置 x
   * @param y 鼠标当前位置 y
   */
  virtual void on_mouse_right_pressed(float x, float y) {
  }

  /**
   * 鼠标右侧松开事件
   * @param x 鼠标当前位置 x
   * @param y 鼠标当前位置 y
   */
  virtual void on_mouse_right_released(float x, float y) {
  }

  /**
   * 鼠标滚轮事件
   * @param delta_x 水平滚动量
   * @param delta_y 垂直滚动量（正值向上，负值向下）
   */
  virtual void on_mouse_wheel(float delta_x, float delta_y) {
  }

  /**
   * 标记脏区域
   * @param reason 模式
   */
  void mark_layout_dirty(LayoutDirty reason = LayoutDirty::Self);

  /**
   * 递归清除所有子控件的悬浮状态
   */
  void clear_hover_state();

public:
  virtual ~Widget();

  /**
   * 不传递任何layout信息
   * 需要子类自行指定
   */
  explicit Widget(Widget *parent = nullptr);

  /**
   * 获取父控件坐标系中的命中检测边界，不包含外边距。
   */
  [[nodiscard]] SkRect hit_test_bounds() const noexcept {
    return SkRect::MakeXYWH(x_, y_, width_, height_);
  }

  /**
   * 控件宽度
   */
  [[nodiscard]] float width() const noexcept {
    return width_;
  }

  /**
   * 控件x坐标
   */
  [[nodiscard]] float x() const noexcept {
    return x_;
  }

  /**
   * 控件y坐标
   */
  [[nodiscard]] float y() const noexcept {
    return y_;
  }

  /**
   * 控件高度
   */
  [[nodiscard]] float height() const noexcept {
    return height_;
  }

  /** 获取控件本地边界，以 (0, 0) 为原点，不包含外边距。 */
  [[nodiscard]] SkRect local_bounds() const noexcept {
    return SkRect::MakeWH(width_, height_);
  }

  /**
   * 获取该控件所有子控件
   * @return child 合集
   */
  [[nodiscard]] std::vector<Widget *> &children() noexcept {
    return children_;
  }

  /**
   * 获取控件树的根控件
   * @return 根控件
   */
  [[nodiscard]] Widget *root_widget() noexcept;

  /**
   * 获取外边距，默认四边均为零。
   * @return 父布局在控件周围预留的间距
   */
  [[nodiscard]] Insets margin() const noexcept {
    return margin_;
  }

  /**
   * 获取控件尺寸约束
   * @return 约束合集
   */
  [[nodiscard]] const SizeConstraints &size_constraints() const noexcept {
    return size_constraints_;
  }

  /** 获取控件在 VBox 布局中的水平对齐方式。 */
  [[nodiscard]] HorizontalAlignment horizontal_alignment() const noexcept {
    return horizontal_;
  }

  /** 获取控件在 HBox 布局中的垂直对齐方式。 */
  [[nodiscard]] VerticalAlignment vertical_alignment() const noexcept {
    return vertical_;
  }

  /**
   * 设置控件在 VBox 布局中的水平对齐方式，并触发重新布局。
   * @param alignment 水平对齐方式
   */
  void set_horizontal_alignment(HorizontalAlignment alignment) noexcept;

  /**
   * 设置控件在 HBox 布局中的垂直对齐方式，并触发重新布局。
   * @param alignment 垂直对齐方式
   */
  void set_vertical_alignment(VerticalAlignment alignment) noexcept;

  /**
   * 是否显示控件
   */
  [[nodiscard]] bool visible() const noexcept {
    return visible_;
  }

  /**
   * 设置控件可见性
   */
  void set_visible(const bool v) noexcept {
    visible_ = v;
  }

  /**
   * 获取布局指针-不拥有所有权
   * @return 布局指针
   */
  [[nodiscard]] Layout<Widget> *layout() const {
    return layout_.get();
  }

  /**
   * 获取当前控件所属指针
   * @return 窗口指针
   */
  [[nodiscard]] WindowBase *window();

  /**
   * 坐标是否被控件包含
   */
  [[nodiscard]] bool contains(float x, float y) const;

  /**
   * 移除布局
   */
  void remove_layout();

  /**
   * 设置控件几何状态
   * @param rect 控件所占rect，基于父控件
   */
  void set_geometry(const SkRect &rect) noexcept;

  /**
   * 设置控件几何状态
   * @param x 相对于父控件：x
   * @param y 相对于父控件: y
   * @param width 控件宽度
   * @param height 控件高度
   */
  void set_geometry(float x, float y, float width, float height) noexcept;

  /**
   * 设置四边相同的外边距，并触发父控件重新布局；手动几何不应用外边距。
   * @param margin 四边外边距，可为负值
   */
  void set_margin(float margin) noexcept;

  /**
   * 设置各边外边距，并触发父控件重新布局；外边距不参与绘制和命中检测。
   * @param insets 各边外边距，可为负值
   */
  void set_margin(const Insets &insets) noexcept;

  /**
   * 设置布局
   */
  template <typename LayoutType>
  requires std::derived_from<LayoutType, Layout<Widget>>
  void set_layout();

  /**
   * 设置最大尺寸
   * @param w 最大宽度
   * @param h 最大高度
   */
  void set_max_size(float w, float h) noexcept;

  /**
   * 设置最小尺寸
   * @param w 最小宽度
   * @param h 最小高度
   */
  void set_min_size(float w, float h) noexcept;

  /**
   * 设置最大宽度
   */
  void set_max_width(float w) noexcept;

  /**
   * 设置最小宽度
   */
  void set_min_width(float w) noexcept;

  /**
   * 设置最大高度
   */
  void set_max_height(float h) noexcept;

  /**
   * 设置最小高度
   */
  void set_min_height(float h) noexcept;

  /**
   * 修改控件的宽高
   * @param width 控件宽度
   * @param height 控件高度
   */
  void resize(float width, float height) noexcept;

  /**
   * 移动控件位置
   * @param x 新的坐标
   * @param y 新的坐标
   */
  void move(float x, float y) noexcept;

  /**
   * 移动控件位置
   * @param point 目标点
   */
  void move(const SkPoint &point) noexcept;

  /**
   * 设置控件x坐标
   */
  void set_x(float x) noexcept;

  /**
   * 设置控件y坐标
   */
  void set_y(float y) noexcept;

  /**
   * 控件长什么样？
   */
  virtual void paint(SkCanvas *canvas) {
  }

  /**
   * 绘制所有控件
   */
  virtual void render(SkCanvas *canvas);

  /**
   * 添加控件
   * @param widget 控件指针-可为空
   */
  virtual void add_widget(Widget *widget);

  /**
   * 更新布局
   */
  void update_layout();

  /**
   * 更新孩子布局
   */
  virtual void layout_children();

  /**
   * 开启一段动画
   * @tparam ptr 成员函数指针
   * @tparam T 参数类型
   * @param from 起始值
   * @param to 目标值
   * @param duration 持续时间
   * @param bezier 可选贝塞尔曲线
   */
  template <auto ptr, typename T, typename... Bezier>
  void start_animation(const T &from, const T &to, float duration, Bezier&&... bezier) noexcept {
    using ClassType = meta::member_class_t<decltype(ptr)>;
    animation_manager->start<ptr>(from, to, duration, std::forward<Bezier>(bezier)..., static_cast<ClassType*>(this));
  }

  /**
   * 开启一段动画
   * @tparam T 参数类型
   * @param from 起始值
   * @param to 目标值
   * @param duration 持续时间
   * @param value 待更新的值
   * @param bezier 可选贝塞尔曲线
   */
  template <typename T, typename... Bezier>
  void start_animation(const T &from, const T &to, float duration, T *value, Bezier&&... bezier) noexcept {
    animation_manager->start(from, to, duration, value, std::forward<Bezier>(bezier)...);
  }
};

void Widget::add_widget(Widget *widget) {
  if (widget == nullptr) {
    return;
  }
  widget->parent_ = this;
  children_.push_back(widget);
}

void Widget::render(SkCanvas *canvas) {
  canvas->save();

  // 变换到self_box
  canvas->translate(x_, y_);

  // 绘制自身
  paint(canvas);

  // 子控件位置已由父布局扣除其外边距，无需再次平移。
  for (auto *const child : children_) {
    if (child->visible_) {
      child->render(canvas);
    }
  }
  canvas->restore();
}

void Widget::mark_layout_dirty(const LayoutDirty reason) {
  if (layout_dirty_ != LayoutDirty::None) {
    // 已经标记过，不重复传播
    return;
  }

  layout_dirty_ |= reason;

  // 传播标记
  if (parent_ != nullptr) {
    parent_->mark_layout_dirty(LayoutDirty::Children);
  }
}

void Widget::update_layout() {
  // 无需更新
  if (layout_dirty_ == LayoutDirty::None) {
    return;
  }

  layout_children();

  // 清空 dirty
  layout_dirty_ = LayoutDirty::None;
}

void Widget::layout_children() {
  // 重新布局
  if (layout_) {
    layout_->apply(*this);
  }

  // 更新子控件布局
  for (auto *const child : this->children_) {
    child->update_layout();
  }
}

Widget::~Widget() {
  for (auto *const child : children_) {
    // 本控件析构不需要再次处理自身
    child->parent_ = nullptr;
    delete child;
  }
  if (parent_) {
    auto &v = parent_->children_;
    if (const auto it = std::ranges::find(v, this); it != v.end()) {
      v.erase(it);
    }
  }
}

Widget::Widget(Widget *parent) : parent_(parent) {
  if (parent != nullptr) {
    parent_->add_widget(this);
  }
}

Widget *Widget::root_widget() noexcept {
  auto *root = this;
  while (root->parent_ != nullptr) {
    root = root->parent_;
  }
  return root;
}

WindowBase *Widget::window() {
  if (window_) {
    return window_;
  }
  auto *parent = parent_;
  while (parent) {
    if (auto *const w = dynamic_cast<WindowBase *>(parent)) {
      return window_ = w;
    }
    parent = parent->parent_;
  }

  throw std::runtime_error("Can not find a window!");
}

bool Widget::contains(const float x, const float y) const {
  return x >= x_ && x < x_ + width_ && y >= y_ && y < y_ + height_;
}

void Widget::set_geometry(const SkRect &rect) noexcept {
  if (parent_ && parent_->has_layout_) {
    return;
  }

  apply_layout_geometry(rect);
}

// clang-format off
void Widget::set_geometry(const float x, const float y, const float width, const float height) noexcept {
  set_geometry(SkRect::MakeXYWH(x, y, width, height));
}
// clang-format on

void Widget::apply_layout_geometry(const SkRect &rect) noexcept {
  if (x_ == rect.x() && y_ == rect.y() && width_ == rect.width() && height_ == rect.height()) {
    return;
  }

  x_ = rect.x();
  y_ = rect.y();
  width_ = rect.width();
  height_ = rect.height();
  mark_layout_dirty(LayoutDirty::Self);
}

void Widget::set_margin(const float margin) noexcept {
  set_margin(Insets(margin));
}

void Widget::set_margin(const Insets &insets) noexcept {
  if (margin_.left == insets.left && margin_.top == insets.top
      && margin_.right == insets.right && margin_.bottom == insets.bottom) {
    return;
  }

  margin_ = insets;
  mark_layout_dirty(LayoutDirty::Self);
}

void Widget::set_horizontal_alignment(const HorizontalAlignment alignment) noexcept {
  if (horizontal_ == alignment) return;

  horizontal_ = alignment;
  mark_layout_dirty();
}

void Widget::set_vertical_alignment(const VerticalAlignment alignment) noexcept {
  if (vertical_ == alignment) return;

  vertical_ = alignment;
  mark_layout_dirty();
}

template <typename LayoutType>
requires std::derived_from<LayoutType, Layout<Widget>>
void Widget::set_layout() {
  layout_ = std::make_unique<LayoutType>();
  has_layout_ = true;
  mark_layout_dirty();
}

void Widget::remove_layout() {
  layout_ = nullptr;
  has_layout_ = false;
  mark_layout_dirty();
}

void Widget::set_max_size(const float w, const float h) noexcept {
  size_constraints_.max_w = std::max(w, size_constraints_.min_w);
  size_constraints_.max_h = std::max(h, size_constraints_.min_h);
  mark_layout_dirty();
}

void Widget::set_min_size(const float w, const float h) noexcept {
  size_constraints_.min_w = std::min(w, size_constraints_.max_w);
  size_constraints_.min_h = std::min(h, size_constraints_.max_h);
  mark_layout_dirty();
}

void Widget::set_max_width(const float w) noexcept {
  size_constraints_.max_w = std::max(w, size_constraints_.min_w);
  mark_layout_dirty();
}

void Widget::set_min_width(const float w) noexcept {
  size_constraints_.min_w = std::min(w, size_constraints_.max_w);
  mark_layout_dirty();
}

void Widget::set_max_height(const float h) noexcept {
  size_constraints_.max_h = std::max(h, size_constraints_.min_h);
  mark_layout_dirty();
}

void Widget::set_min_height(const float h) noexcept {
  size_constraints_.min_h = std::min(h, size_constraints_.max_h);
  mark_layout_dirty();
}

void Widget::resize(const float width, const float height) noexcept {
  set_geometry(x_, y_, width, height);
}

void Widget::move(const float x, const float y) noexcept {
  if (parent_ && parent_->has_layout_ || x == x_ && y == y_) {
    return;
  }

  x_ = x;
  y_ = y;
}

void Widget::move(const SkPoint &point) noexcept {
  move(point.x(), point.y());
}

void Widget::set_x(const float x) noexcept {
  if (parent_ && parent_->has_layout_ || x == x_) {
    return;
  }

  x_ = x;
}

void Widget::set_y(const float y) noexcept {
  if (parent_ && parent_->has_layout_ || y == y_) {
    return;
  }

  y_ = y;
}

void Widget::mouse_move(const float x, const float y) {
  if (!hovered_) {
    hovered_ = true;
    on_mouse_enter(x, y);
  }

  // 再处理move
  on_mouse_move(x, y);

  // 子控件坐标已包含布局产生的外边距偏移。
  for (auto *const child : std::views::reverse(children_)) {
    if (!child->visible_) continue;
    if (child->contains(x, y) || child->is_dragging_) {
      // 鼠标在此子控件内，清除其他兄弟的悬浮状态
      for (auto *const sibling : children_) {
        if (sibling != child && sibling->visible_ && sibling->hovered_) {
          sibling->clear_hover_state();
        }
      }
      child->mouse_move(x - child->x_, y - child->y_);
      return;
    }
  }

  // 鼠标不在任何子控件内，清除所有子控件的悬浮状态
  for (auto *const child : children_) {
    if (child->visible_ && child->hovered_) {
      child->clear_hover_state();
    }
  }
}

void Widget::mouse_left_pressed(const float x, const float y) {
  for (auto *const child : std::views::reverse(children_)) {
    if (child->visible_ && (child->contains(x, y) || child->is_dragging_)) {
      child->mouse_left_pressed(x - child->x_, y - child->y_);
      mouse_capture_ = child;
      break;
    }
  }

  // 使用原坐标处理自身
  on_mouse_left_pressed(x, y);
}

void Widget::mouse_left_released(const float x, const float y) {
  if (mouse_capture_) {
    mouse_capture_->mouse_left_released(x - mouse_capture_->x_, y - mouse_capture_->y_);
    mouse_capture_ = nullptr;
  }

  on_mouse_left_released(x, y);
}

void Widget::mouse_right_pressed(const float x, const float y) {
  for (std::size_t index = children_.size(); index > 0; --index) {
    auto *const child = children_[index - 1];
    if (child->visible_ && child->contains(x, y)) {
      child->mouse_right_pressed(x - child->x_, y - child->y_);
      right_mouse_capture_ = child;
      break;
    }
  }

  // 使用原坐标处理自身
  on_mouse_right_pressed(x, y);
}

void Widget::mouse_right_released(const float x, const float y) {
  if (right_mouse_capture_ != nullptr) {
    right_mouse_capture_->mouse_right_released(
      x - right_mouse_capture_->x_,
      y - right_mouse_capture_->y_
    );
    right_mouse_capture_ = nullptr;
  }

  on_mouse_right_released(x, y);
}

void Widget::mouse_wheel(const float x, const float y, const float delta_x, const float delta_y) {
  // 先让自身处理
  on_mouse_wheel(delta_x, delta_y);

  // 再分发给子控件
  for (auto *const child : children_) {
    if (child->visible_ && child->contains(x, y)) {
      child->mouse_wheel(x - child->x_, y - child->y_, delta_x, delta_y);
      return;
    }
  }
}

void Widget::clear_hover_state() {
  if (hovered_) {
    hovered_ = false;
    on_mouse_leave(0, 0);
  }

  for (auto *const child : children_) {
    if (child->visible_) {
      child->clear_hover_state();
    }
  }
}

} // namespace ui::widgets
