//
// Created by love-yuri on 2026/1/16.
//

export module ui.widgets:base;

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
  Widget *mouse_capture = nullptr; // 正在被点击的控件
  friend Layout<Widget>;           // 友元layout组件
protected:
  Widget *parent_ = nullptr;         // 父控件
  std::vector<Widget *> children_{}; // 子控件列表
  Layout<Widget> *layout_ = nullptr; // 布局
  LayoutDirty layout_dirty{};        // 脏布局类型

  /* 控件几何信息 */
  float width_ = 0.f;               // 控件宽度
  float height_ = 0.f;              // 控件高度
  SkRect self_box{};                // 自身box模型 [x, y] 相对于父布局开始
  SkRect content_box{};             // 内容box模型 [pl, pt] 开始
  Insets padding_{};                // 内边距
  SizeConstraints size_constraints; // 布局约束信息

  /**
   * 是否展示 - 手动修改
   */
  std::uint8_t visible : 1 = 1;

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
   * 控件长什么样？
   */
  virtual void paint(SkCanvas *canvas) = 0;

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
  * 重新计算内容box大小
  */
  void updateContentBox();

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
  [[nodiscard]] inline const SkRect& hitTestBounds() const noexcept {
    return self_box;
  }

  /**
   * 获取控件宽度
   */
  [[nodiscard]] inline float width() const noexcept {
    return width_;
  }

  /**
   * 获取控件高度
   */
  [[nodiscard]] inline float height() const noexcept {
    return height_;
  }

  /**
   * 获取控件内容宽度-去除padding区域
   */
  [[nodiscard]] inline float contentWidth() const noexcept {
    return content_box.width();
  }

  /**
   * 获取控件内容宽度-去除padding区域
   */
  [[nodiscard]] inline float contentHeight() const noexcept {
    return content_box.height();
  }

  /**
   * 获取该控件所有子控件
   * @return child 合集
   */
  [[nodiscard]] inline std::vector<Widget*>& children() noexcept {
    return children_;
  }

  /**
   * 获取内边距
   * @return 内边距合集
   */
  [[nodiscard]] inline Insets padding() const noexcept {
    return padding_;
  }

  /**
   * 获取控件尺寸约束
   * @return 约束合集
   */
  [[nodiscard]] inline const SizeConstraints& sizeConstraints() const noexcept {
    return size_constraints;
  }

  /**
   * 设置控件几何状态
   * @param rect 控件所占rect，基于父控件
   */
  inline void setGeometry(const SkRect &rect) noexcept;

  /**
   * 设置控件几何状态
   * @param x 相对于父控件：x
   * @param y 相对于父控件: y
   * @param width 控件宽度
   * @param height 控件高度
   */
  inline void setGeometry(float x, float y, float width, float height) noexcept;

  /**
   * 设置内边距
   */
  inline void setPadding(float padding) noexcept;

  /**
   * 设置内边距
   */
  inline void setPadding(const Insets& insets) noexcept;

  /**
   * 设置最大尺寸
   * @param w 最大宽度
   * @param h 最大高度
   */
  inline void setMaxSize(float w, float h) noexcept;

  /**
   * 设置最大宽度
   */
  inline void setMaxWidth(float w) noexcept;

  /**
   * 设置最大高度
   */
  inline void setMaxHeight(float h) noexcept;

  /**
   * 修改控件的宽高
   * @param width 控件宽度
   * @param height 控件高度
   */
  inline void resize(float width, float height) noexcept;

  /**
   * 移动控件位置
   * @param x 新的坐标
   * @param y 新的坐标
   */
  inline void move(float x, float y) noexcept;

  /**
   * 移动控件位置
   * @param point 目标点
   */
  inline void move(const SkPoint& point) noexcept;

  /**
   * 更新布局
   */
  inline void updateLayout();

  /**
   * 更新孩子布局
   */
  virtual void layoutChildren() {
  }
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
  // 绘制自身
  paint(canvas);

  // 变换后绘制children
  canvas->translate(self_box.x() + padding_.left, self_box.y() + padding_.top);
  for (const auto child : children_) {
    if (child->visible) {
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

  if (parent_ == nullptr || parent_->layout_ == nullptr) {
    // 如果没有parent 或者 parent 没有布局则仅更新自己
    updateLayout();
  } else {
    // 否则标记parent 需要更新
    parent_->markLayoutDirty(LayoutDirty::Children);
  }
}

void Widget::updateContentBox() {
  content_box.setXYWH(
    padding_.left,
    padding_.top,
    width_ - padding_.left - padding_.right,
    height_ - padding_.top - padding_.bottom
  );
}

inline void Widget::updateLayout() {
  // 无需更新
  if (layout_dirty == LayoutDirty::None) {
    return;
  }

  layoutChildren();

  // 清空 dirty
  layout_dirty = LayoutDirty::None;
}

Widget::~Widget() {
  for (const auto child : children_) {
    // 本控件析构不需要再次处理自身
    child->parent_ = nullptr;
    delete child;
  }
  if (parent_ != nullptr) {
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
  parent_->children_.push_back(this);
}

inline void Widget::setGeometry(const SkRect &rect) noexcept {
  this->self_box = rect;
  width_ = rect.width();
  height_ = rect.height();
  updateContentBox();
  markLayoutDirty(LayoutDirty::Self);
}

inline void Widget::setGeometry(const float x, const float y, const float width, const float height) noexcept {
  width_ = width;
  height_ = height;
  self_box.setXYWH(x, y, width, height);
  updateContentBox();
  markLayoutDirty(LayoutDirty::Self);
}

inline void Widget::setPadding(const float padding) noexcept {
  padding_.setAll(padding);
  updateContentBox();
  markLayoutDirty(LayoutDirty::Self);
}

void Widget::setPadding(const Insets& insets) noexcept {
  padding_ = insets;
  updateContentBox();
  markLayoutDirty(LayoutDirty::Self);
}

void Widget::setMaxSize(const float w, const float h) noexcept {
  size_constraints.max_w = std::max(w, size_constraints.min_w);
  size_constraints.max_h = std::max(h, size_constraints.min_h);
}

inline void Widget::setMaxWidth(const float w) noexcept {
  size_constraints.max_w = std::max(w, size_constraints.min_w);
}

inline void Widget::setMaxHeight(const float h) noexcept {
  size_constraints.max_h = std::max(h, size_constraints.min_h);
}

inline void Widget::resize(const float width, const float height) noexcept {
  width_ = width;
  height_ = height;
  self_box.setXYWH(self_box.x(), self_box.y(), width, height);
  updateContentBox();
  markLayoutDirty(LayoutDirty::Self);
}

inline void Widget::move(const float x, const float y) noexcept {
  if (parent_ != nullptr && parent_->layout_ != nullptr) {
    return;
  }
  self_box.setXYWH(x, y, width_, height_);
}

inline void Widget::move(const SkPoint &point) noexcept {
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
    if (child->visible) {
      if (child->self_box.contains(child_x, child_y) || child->is_dragging) {
        child->MouseMove(
          child_x - child->self_box.x(),
          child_y - child->self_box.y()
        );
      } else if (child->hovered_) {
        child->hovered_ = false;
        child->onMouseLeave(
          child_x - child->self_box.x(),
          child_y - child->self_box.y()
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
    if (child->visible && child->self_box.contains(child_x, child_y)) {
      child->MouseLeftPressed(
        child_x - child->self_box.x(),
        child_y - child->self_box.y()
      );
      mouse_capture = child;
      return;
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
      child_x - mouse_capture->self_box.x(),
      child_y - mouse_capture->self_box.y()
    );
    mouse_capture = nullptr;
  } else {
    onMouseLeftReleased(x, y);
  }
}

} // namespace ui::widgets