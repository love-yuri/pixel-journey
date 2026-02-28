//
// Created by love-yuri on 2026/1/16.
//

export module ui.widgets:base;

import :window_base;
import skia.api;
import yuri_log;
import ui.layout;
import std;
import signal;

using namespace skia;
using namespace ui::layout;

export namespace ui::widgets {

/**
 * 控件基类
 */
class Widget {
  WindowBase* window_ = nullptr;   // 窗口指针
  Widget *mouse_capture = nullptr; // 正在被点击的控件
  friend Layout<Widget>;           // 友元layout组件

protected:
  Widget *parent_ = nullptr;                         // 父控件
  std::vector<Widget *> children_{};                 // 子控件列表
  LayoutDirty layout_dirty{};                        // 脏布局类型
  std::unique_ptr<Layout<Widget>> layout_ = nullptr; // 布局

  /* 控件几何信息 */
  float x_ = 0.f;                   // 控件x 坐标
  float y_ = 0.f;                   // 控件y 坐标
  float width_ = 0.f;               // 控件宽度
  float height_ = 0.f;              // 控件高度
  Insets padding_{};                // 内边距
  SizeConstraints size_constraints; // 布局约束信息

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
  std::uint8_t is_dragging : 1 = 0;

  /**
  * 判断控件是否存在layout - 手动修改
  */
  std::uint8_t has_layout : 1 = 0;

  /**
   * 鼠标移动事件
   * @param x x: 相对于左上角
   * @param y y: 相对于左上角
   */
  void MouseMove(float x, float y);

  /**
   * 鼠标左侧点击事件
   */
  void MouseLeftPressed(float x, float y);

  /**
   * 鼠标左侧松开事件
   */
  void MouseLeftReleased(float x, float y);

  /**
   * 鼠标移动事件
   * @param x 鼠标当前位置 x
   * @param y 相对于左上角
   */
  virtual void onMouseMove(float x, float y) {
  }

  /**
   * 鼠标进入
   * @param x 鼠标当前位置 x
   * @param y 鼠标当前位置 y
   */
  virtual void onMouseEnter(float x, float y) {
  }

  /**
   * 鼠标移出
   * @param x 鼠标当前位置 x
   * @param y 鼠标当前位置 y
   */
  virtual void onMouseLeave(float x, float y) {
  }

  /**
   * 鼠标左侧点击事件
   * @param x 鼠标当前位置 x
   * @param y 鼠标当前位置 y
   */
  virtual void onMouseLeftPressed(float x, float y) {
  }

  /**
   * 鼠标左侧松开事件
   * @param x 鼠标当前位置 x
   * @param y 鼠标当前位置 y
   */
  virtual void onMouseLeftReleased(float x, float y) {
  }

  /**
   * 标记脏区域
   * @param reason 模式
   */
  void markLayoutDirty(LayoutDirty reason = LayoutDirty::Self);

  /**
   * 设置控件几何状态-内部或者给布局系统使用
   * @param widget 待设置的控件
   * @param rect 控件所占rect，基于父控件
   */
  static void setGeometry(Widget* widget, const SkRect &rect) noexcept;

  /**
   * 设置控件几何状态-内部或者给布局系统使用
   * @param widget 待设置的控件
   * @param x 相对于父控件：x
   * @param y 相对于父控件: y
   * @param width 控件宽度
   * @param height 控件高度
   */
  static void setGeometry(Widget* widget, float x, float y, float width, float height) noexcept;

public:
  virtual ~Widget();

  /**
   * 不传递任何layout信息
   * 需要子类自行指定
   */
  explicit Widget(Widget* parent = nullptr);

  /**
   * 命中检测边界
   */
  [[nodiscard]] SkRect hitTestBounds() const noexcept {
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

  /**
   * 获取控件内容宽度-去除padding区域
   */
  [[nodiscard]] float contentWidth() const noexcept {
    return width_ - padding_.left - padding_.right;
  }

  /**
   * 获取控件内容宽度-去除padding区域
   */
  [[nodiscard]] float contentHeight() const noexcept {
    return height_ - padding_.top - padding_.bottom;
  }

  /**
   * 获取该控件所有子控件
   * @return child 合集
   */
  [[nodiscard]] std::vector<Widget*>& children() noexcept {
    return children_;
  }

  /**
   * 获取内边距
   * @return 内边距合集
   */
  [[nodiscard]] Insets padding() const noexcept {
    return padding_;
  }

  /**
   * 获取控件尺寸约束
   * @return 约束合集
   */
  [[nodiscard]] const SizeConstraints& sizeConstraints() const noexcept {
    return size_constraints;
  }

  /**
   * 是否显示控件
   */
  [[nodiscard]] bool visible() const noexcept {
    return visible_;
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
  [[nodiscard]] WindowBase * window();

  /**
   * 坐标是否被控件包含
   */
  [[nodiscard]] bool contains(float x, float y) const;

  /**
   * 移除布局
   */
  void removeLayout();

  /**
   * 设置控件几何状态
   * @param rect 控件所占rect，基于父控件
   */
  void setGeometry(const SkRect &rect) noexcept;

  /**
   * 设置控件几何状态
   * @param x 相对于父控件：x
   * @param y 相对于父控件: y
   * @param width 控件宽度
   * @param height 控件高度
   */
  void setGeometry(float x, float y, float width, float height) noexcept;

  /**
   * 设置内边距
   */
  void setPadding(float padding) noexcept;

  /**
   * 设置内边距
   */
  void setPadding(const Insets& insets) noexcept;

  /**
   * 设置布局
   */
  template <typename LayoutType>
  void setLayout();

  /**
   * 设置最大尺寸
   * @param w 最大宽度
   * @param h 最大高度
   */
  void setMaxSize(float w, float h) noexcept;

  /**
   * 设置最小尺寸
   * @param w 最小宽度
   * @param h 最小高度
   */
  void setMinSize(float w, float h) noexcept;

  /**
   * 设置最大宽度
   */
  void setMaxWidth(float w) noexcept;

  /**
   * 设置最小宽度
   */
  void setMinWidth(float w) noexcept;

  /**
   * 设置最大高度
   */
  void setMaxHeight(float h) noexcept;

  /**
   * 设置最小高度
   */
  void setMinHeight(float h) noexcept;

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
  void move(const SkPoint& point) noexcept;

  /**
   * 控件长什么样？
   */
  virtual void paint(SkCanvas *canvas){}

  /**
   * 绘制所有控件
   */
  virtual void render(SkCanvas *canvas);

  /**
   * 添加控件
   * @param widget 控件指针-可为空
   */
  virtual void addWidget(Widget * widget);

  /**
   * 更新布局
   */
  void updateLayout();

  /**
   * 更新孩子布局
   */
  virtual void layoutChildren();
};

void Widget::addWidget(Widget * widget) {
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

  // 变换后绘制children
  canvas->translate(padding_.left, padding_.top);
  for (const auto child : children_) {
    if (child->visible_) {
      child->render(canvas);
    }
  }
  canvas->restore();
}

void Widget::markLayoutDirty(const LayoutDirty reason) {
  if (layout_dirty != LayoutDirty::None) {
    // 已经标记过，不重复传播
    return;
  }

  layout_dirty |= reason;

  // 传播标记
  if (parent_ != nullptr) {
    parent_->markLayoutDirty(LayoutDirty::Children);
  }
}


void Widget::setGeometry(Widget *widget, const SkRect &rect) noexcept {
  widget->x_ = rect.x();
  widget->y_ = rect.y();
  widget->width_ = rect.width();
  widget->height_ = rect.height();
  widget->markLayoutDirty(LayoutDirty::Self);
}

void Widget::setGeometry(Widget *widget, const float x, const float y, const float width, float const height) noexcept {
  widget->x_ = x;
  widget->y_ = y;
  widget->width_ = width;
  widget->height_ = height;
  widget->markLayoutDirty(LayoutDirty::Self);
}

void Widget::updateLayout() {
  // 无需更新
  if (layout_dirty == LayoutDirty::None) {
    return;
  }

  layoutChildren();

  // 清空 dirty
  layout_dirty = LayoutDirty::None;
}

void Widget::layoutChildren() {
  // 重新布局
  if (layout_) {
    layout_->apply();
  }

  // 更新子控件布局
  for (const auto child : this->children_) {
    child->updateLayout();
  }
}

Widget::~Widget() {
  for (const auto child : children_) {
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

Widget::Widget(Widget *parent): parent_(parent) {
  if (parent == nullptr) {
    return;
  }

  parent_->addWidget(this);
}

WindowBase *Widget::window() {
  if (window_) {
    return window_;
  }
  auto parent = parent_;
  while (parent) {
    if (dynamic_cast<WindowBase*>(parent)) {
      return window_ = dynamic_cast<WindowBase*>(parent);
    }
    parent = parent->parent_;
  }

  throw std::runtime_error("Can not find a window!");
}

bool Widget::contains(const float x, const float y) const {
  return x >= x_ && x < x_ + width_ && y >= y_ && y < y_ + height_;
}

void Widget::setGeometry(const SkRect &rect) noexcept {
  if (parent_ && parent_->has_layout) {
    return;
  }

  x_ = rect.x();
  y_ = rect.y();
  width_ = rect.width();
  height_ = rect.height();
  markLayoutDirty(LayoutDirty::Self);
}

void Widget::setGeometry(const float x, const float y, const float width, const float height) noexcept {
  if (parent_ && parent_->has_layout) {
    return;
  }

  x_ = x;
  y_ = y;
  width_ = width;
  height_ = height;
  markLayoutDirty(LayoutDirty::Self);
}

void Widget::setPadding(const float padding) noexcept {
  padding_.setAll(padding);
  markLayoutDirty(LayoutDirty::Self);
}

void Widget::setPadding(const Insets& insets) noexcept {
  padding_ = insets;
  markLayoutDirty(LayoutDirty::Self);
}

template <typename LayoutType>
void Widget::setLayout() {
  static_assert(std::is_base_of_v<Layout<Widget>, LayoutType>, "LayoutType must inherit from Layout<Widget>");
  layout_ = std::make_unique<LayoutType>(this);
  has_layout = true;
}

void Widget::removeLayout() {
  layout_ = nullptr;
  has_layout = false;
}

void Widget::setMaxSize(const float w, const float h) noexcept {
  size_constraints.max_w = std::max(w, size_constraints.min_w);
  size_constraints.max_h = std::max(h, size_constraints.min_h);
}

void Widget::setMinSize(const float w, const float h) noexcept {
  size_constraints.min_w = std::min(w, size_constraints.max_w);
  size_constraints.min_h = std::min(h, size_constraints.max_h);
}

void Widget::setMaxWidth(const float w) noexcept {
  size_constraints.max_w = std::max(w, size_constraints.min_w);
}

void Widget::setMinWidth(const float w) noexcept {
  size_constraints.min_w = std::min(w, size_constraints.max_w);
}

void Widget::setMaxHeight(const float h) noexcept {
  size_constraints.max_h = std::max(h, size_constraints.min_h);
}

void Widget::setMinHeight(const float h) noexcept {
  size_constraints.min_h = std::min(h, size_constraints.max_h);
}

void Widget::resize(const float width, const float height) noexcept {
  if (parent_ && parent_->has_layout) {
    return;
  }

  width_ = width;
  height_ = height;

  markLayoutDirty(LayoutDirty::Self);
}

void Widget::move(const float x, const float y) noexcept {
  if (parent_ && parent_->has_layout) {
    return;
  }

  x_ = x;
  y_ = y;
}

void Widget::move(const SkPoint &point) noexcept {
  move(point.x(), point.y());
}

void Widget::MouseMove(const float x, const float y) {
  if (!hovered_) {
    hovered_ = true;
    onMouseEnter(x, y);
  }

  // 再处理move
  onMouseMove(x, y);

  // 减去padding 再开始判定
  const auto child_x = x - padding_.left;
  const auto child_y = y - padding_.top;

  for (const auto child : children_) {
    if (child->visible_) {
      if (child->contains(child_x, child_y) || child->is_dragging) {
        child->MouseMove(
          child_x - child->x_,
          child_y - child->y_
        );
      } else if (child->hovered_) {
        child->hovered_ = false;
        child->onMouseLeave(
          child_x - child->x_,
          child_y - child->y_
        );
      }
    }
  }
}

void Widget::MouseLeftPressed(const float x, const float y) {
  // 减去padding 再开始判定
  const auto child_x = x - padding_.left;
  const auto child_y = y - padding_.top;

  for (const auto child : children_) {
    if (child->visible_ && child->contains(child_x, child_y)) {
      child->MouseLeftPressed(
        child_x - child->x_,
        child_y - child->y_
      );
      mouse_capture = child;
      break;
    }
  }

  // 使用原坐标处理自身
  onMouseLeftPressed(x, y);
}

void Widget::MouseLeftReleased(const float x, const float y) {
  // 减去padding 再开始判定
  const auto child_x = x - padding_.left;
  const auto child_y = y - padding_.top;

  if (mouse_capture) {
    mouse_capture->MouseLeftReleased(
      child_x - mouse_capture->x_,
      child_y - mouse_capture->y_
    );
    mouse_capture = nullptr;
  }

  onMouseLeftReleased(x, y);
}

} // namespace ui::widgets