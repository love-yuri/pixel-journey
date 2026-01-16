//
// Created by love-yuri on 2026/1/16.
//

export module ui.widgets:base;

import skia.api;
import yuri_log;
import std;

using namespace skia;

export namespace ui::widgets {

/**
 * 控件基类
 */
class Widget {
  std::vector<Widget*> children_; // 孩子
protected:
  Widget *parent_; // 父控件
  SkRect rect_;    // 控件所占矩形空间
  float width_;    // 控件宽度
  float height_;   // 控件高度

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

public:
  virtual ~Widget();

  /**
   * 使用rect和parent初始化控件
   * @param rect 控件位置尺寸
   * @param parent 父控件
   */
  explicit Widget(const SkRect &rect, Widget* parent);

  /**
   * 不传递位置，默认使用父控件的
   */
  explicit Widget(Widget* parent);

  /**
   * 命中检测边界
   * @return 默认返回rect_
   */
  virtual SkRect hitTestBounds() const;

  /**
   * 控件高度
   */
  virtual float height() const {
    return height_;
  }

  /**
   * 控件宽度
   */
  virtual float width() const {
    return width_;
  }

  /**
   * 鼠标移动事件
   * @param x x: 相对于左上角
   * @param y y: 相对于左上角
   */
  virtual void onMouseMove(float x, float y) {
  }

  /**
   * 鼠标进入
   */
  virtual void onMouseEnter() {
  }

  /**
   * 鼠标移出
   */
  virtual void onMouseLeave() {
  }

  /**
   * 鼠标左侧点击事件
   */
  virtual void onMouseLeftPressed() {
  }

  /**
   * 鼠标左侧松开事件
   */
  virtual void onMouseLeftReleased() {
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
  paint(canvas);

  for (const auto child : children_) {
    child->render(canvas);
  }
}

Widget::~Widget() {
  for (const auto child : children_) {
    delete child;
  }
  if (parent_ != nullptr) {
    auto &v = parent_->children_;
    v.erase(std::ranges::remove(v, this).begin(), v.end());
  }
}

Widget::Widget(const SkRect &rect, Widget *parent)
: parent_(parent), rect_(rect), width_(rect.width()), height_(rect.height()) {
  if (parent == nullptr) {
    return;
  }
  parent_->children_.push_back(this);
}

Widget::Widget(Widget *parent): Widget(
  parent != nullptr ? parent->rect_ : SkRect::MakeEmpty(), parent) {
}

SkRect Widget::hitTestBounds() const {
  return rect_;
}

} // namespace ui::widgets