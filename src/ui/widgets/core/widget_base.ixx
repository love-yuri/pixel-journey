//
// Created by love-yuri on 2026/1/16.
//

export module ui.widgets:base;

import skia.api;
import yuri_log;
import ui.core;
import std;

using namespace skia;

export namespace ui::widgets {

/**
 * 控件基类
 */
class Widget: public core::RectTransform {
  std::vector<Widget*> children_; // 子控件列表

protected:
  Widget *parent_ = nullptr;          // 父控件何
  bool visible = true;                // 是否展示

  /**
   * 添加控件
   * @param widget 控件指针-可为空
   */
  void addWidget(Widget * widget);

  /**
   * 控件长什么样？
   */
  virtual void paint(SkCanvas *canvas) = 0;

  /**
   * 绘制所有控件
   */
  virtual void render(SkCanvas *canvas);

  /**
   * 鼠标移动事件
   * @param x x: 相对于左上角
   * @param y y: 相对于左上角
   */
  void MouseMove(float x, float y);

  /**
   * 鼠标进入
   */
  void MouseEnter(float x, float y);

  /**
   * 鼠标移出
   */
  void MouseLeave(float x, float y);

  /**
   * 鼠标左侧点击事件
   */
  void MouseLeftPressed(float x, float y);

  /**
   * 鼠标左侧松开事件
   */
  void MouseLeftReleased(float x, float y);

public:
  ~Widget() override;

  /**
   * 不传递任何layout信息
   * 需要子类自行指定
   */
  explicit Widget(Widget* parent);

  /**
   * 命中检测边界
   * @return 默认返回rect_
   */
  [[nodiscard]] virtual const SkRect& hitTestBounds() const;

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
  canvas->translate(local_rect_.x(), local_rect_.y());
  paint(canvas);
  for (const auto child : children_) {
    child->render(canvas);
  }
  canvas->restore();
}

/**
 * 重载==防止clangd报错（实际能跑）
 * @param it it
 * @param end end
 * @return 是否相等
 */
bool operator==(const std::vector<Widget *>::iterator & it, const std::vector<Widget *>::iterator & end) {
  return it.base() == end.base();
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

const SkRect& Widget::hitTestBounds() const {
  return local_rect_;
}

void Widget::MouseMove(float x, float y) {
  x -= local_rect_.x();
  y -= local_rect_.y();
  onMouseMove(x, y);
  for (const auto child : children_) {
    if (child->visible && child->hitTestBounds().contains(x, y)) {
      child->MouseMove(x, y);
      break;
    }
  }
}

void Widget::MouseEnter(float x, float y) {
  x -= local_rect_.x();
  y -= local_rect_.y();
  onMouseEnter(x, y);
  for (const auto child : children_) {
    if (child->visible && child->hitTestBounds().contains(x, y)) {
      child->MouseEnter(x, y);
      break;
    }
  }
}

void Widget::MouseLeave(float x, float y) {
  onMouseLeave(x, y);
  x -= local_rect_.x();
  y -= local_rect_.y();
  for (const auto child : children_) {
    if (child->visible && child->hitTestBounds().contains(x, y)) {
      child->MouseLeave(x, y);
      break;
    }
  }
}

void Widget::MouseLeftPressed(float x, float y) {
  x -= local_rect_.x();
  y -= local_rect_.y();
  onMouseLeftPressed(x, y);
  for (const auto child : children_) {
    if (child->visible && child->hitTestBounds().contains(x, y)) {
      child->MouseLeftPressed(x, y);
      break;
    }
  }
}

void Widget::MouseLeftReleased(float x, float y) {
  x -= local_rect_.x();
  y -= local_rect_.y();
  onMouseLeftReleased(x, y);
  for (const auto child : children_) {
    if (child->visible && child->hitTestBounds().contains(x, y)) {
      child->MouseLeftReleased(x, y);
      break;
    }
  }
}

} // namespace ui::widgets