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

export namespace ui::widgets {

enum class LayoutDirty : std::uint8_t {
  None = 0,            // 没有改变
  Self = 1 << 0,       // 自身尺寸/位置改变
  Children = 1 << 1    // 子节点改变
};

inline LayoutDirty operator|(LayoutDirty a, LayoutDirty b) {
  return static_cast<LayoutDirty>(
      static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b)
  );
}

inline LayoutDirty& operator|=(LayoutDirty &a, LayoutDirty b) {
  a = a | b;
  return a;
}

/**
 * 控件基类
 */
class Widget {
  Widget* mouse_capture = nullptr;    // 正在被点击的控件
protected:
  std::vector<Widget*> children_{};   // 子控件列表

  // 自身box模型，不包含margin
  SkRect self_box {0, 0, 0, 0 };
  LayoutDirty layout_dirty{};        // 布局是否变脏
  float width_ = 0;                   // 控件宽度
  float height_ = 0;                  // 控件高度
  Widget *parent_ = nullptr;          // 父控件何
  bool visible = true;                // 是否展示 - 手动修改

  /**
   * 是否正在被hover - 自动修改
   * 判断鼠标是否在控件内，该属性自动修改，无需手动赋值
   */
  bool hovered_ = false;

  /**
   * 鼠标是否正在被拖拽 - 手动修改
   * 开启后move事件将不再检查是否在控件内
   */
  bool is_dragging = false;

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
  void addWidget(Widget * widget);

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
   * 获取该控件所有子控件
   * @return child 合集
   */
  inline std::vector<Widget*>& children() noexcept;

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
  canvas->translate(self_box.x(), self_box.y());
  paint(canvas);
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

  layout_dirty = reason;

  // 子节点变化通常影响父容器布局
  if (parent_ != nullptr) {
    parent_->markLayoutDirty(LayoutDirty::Children);
  }
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

inline std::vector<Widget *> &Widget::children() noexcept {
  return children_;
}

inline void Widget::setGeometry(const SkRect &rect) noexcept {
  this->self_box = rect;
  width_ = rect.width();
  height_ = rect.height();
  markLayoutDirty(LayoutDirty::Self);
}

inline void Widget::setGeometry(const float x, const float y, const float width, const float height) noexcept {
  width_ = width;
  height_ = height;
  self_box.setXYWH(x, y, width, height);
  markLayoutDirty(LayoutDirty::Self);
}

inline void Widget::resize(const float width, const float height) noexcept {
  width_ = width;
  height_ = height;
  self_box.setXYWH(self_box.x(), self_box.y(), width, height);
  markLayoutDirty(LayoutDirty::Self);
}

inline void Widget::move(const float x, const float y) noexcept {
  self_box.setXYWH(x, y, width_, height_);
}

inline void Widget::move(const SkPoint &point) noexcept {
  move(point.x(), point.y());
}

void Widget::MouseMove(float x, float y) {
  x -= self_box.x();
  y -= self_box.y();

  // 先处理enter
  if (!hovered_) {
    hovered_ = true;
    onMouseEnter(x, y);
  }

  // 再处理move
  onMouseMove(x, y);
  for (const auto child : children_) {
    if (child->visible) {
      if (child->hitTestBounds().contains(x, y) || child->is_dragging) {
        child->MouseMove(x, y);
      } else if (child->hovered_) {
        child->hovered_ = false;
        child->onMouseLeave(x, y);
      }
    }
  }
}

void Widget::MouseLeftPressed(float x, float y) {
  x -= self_box.x();
  y -= self_box.y();
  for (const auto child : children_) {
    if (child->visible && child->hitTestBounds().contains(x, y)) {
      child->MouseLeftPressed(x, y);
      mouse_capture = child;
      return;
    }
  }

  onMouseLeftPressed(x, y);
}

void Widget::MouseLeftReleased(float x, float y) {
  x -= self_box.x();
  y -= self_box.y();

  if (mouse_capture) {
    mouse_capture->MouseLeftReleased(x, y);
    mouse_capture = nullptr;
  } else {
    onMouseLeftReleased(x, y);
  }
}

} // namespace ui::widgets