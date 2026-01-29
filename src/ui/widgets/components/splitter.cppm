//
// Created by love-yuri on 2026/1/29.
//

export module ui.widgets:splitter;

import std;
import skia;
import yuri_log;
import ui.layout;
import ui.render;
import ui.animation;
import :base;

using namespace ui::render;
using namespace ui::layout;
using namespace skia;
using namespace ui::animation;

export namespace ui::widgets {

class Splitter : public Widget {
  SkRect handle_rect{};    // 中间拖动条
  float handle_width = 4;  // 拖动条默认宽度
  float handle_x = 200;    // 拖动条默认x
  SkPoint last_point{};    // 上次点击点
  bool is_clicked = false; // 是否被点击

public:
  explicit Splitter(Widget *parent) : Widget(parent) {
    has_layout = true;
  }

protected:
  void render(SkCanvas *canvas) override;
  void addWidget(Widget *widget) override;
  void paint(SkCanvas *canvas) override;
  void layoutChildren() override;
  void onMouseMove(float x, float y) override;
  void onMouseLeftPressed(float x, float y) override;
  void onMouseLeftReleased(float x, float y) override;

private:
  // 布局方式
  enum SplitterOrientation {
    Vertical,
    Horizontal
  };

  SplitterOrientation orientation_ = Horizontal;
  SkPaint sk_paint = PaintDesc {
    .color = skia_colors::pink,
    .style = SkPaint::kFill_Style
  };
};

void Splitter::render(SkCanvas *canvas) {
  canvas->save();
  // 绘制自身
  paint(canvas);

  // 变换后绘制children
  canvas->translate(self_box.x() + padding_.left, self_box.y() + padding_.top);

  if (children_.size() > 1) {
    // 绘制左侧控件
    children_[0]->render(canvas);

    // 绘制右侧控件
    children_[1]->render(canvas);

    // 绘制间隔条
    canvas->drawRect(handle_rect, sk_paint);
  } else if (!children_.empty()) {
    children_[0]->render(canvas);
  }

  canvas->restore();
}

void Splitter::addWidget(Widget *widget) {
  Widget::addWidget(widget);
  widget->setGeometry(content_box);
}

void Splitter::paint(SkCanvas *canvas) {
}

void Splitter::layoutChildren() {
  handle_rect.setXYWH(handle_x, 0, handle_width, contentHeight());

  if (children_.empty() || children_.size() > 2) {
    return;
  }

  if (children_.size() == 1) {
    children_[0]->setGeometry(children_[0], content_box);
    return;
  }

  const auto left = children_[0], right = children_[1];
  left->setGeometry(left, 0, 0, handle_x, height_);
  right->setGeometry(right, handle_x + handle_width, 0, width_ - handle_x - handle_width, height_);

  // 更新子控件布局
  for (const auto child : this->children_) {
    child->updateLayout();
  }
}

void Splitter::onMouseMove(const float x, const float y) {
  if (handle_rect.contains(x, y)) {
    window()->setCursor(CursorType::HResize);
  } else {
    window()->setCursor(CursorType::Arrow);
  }

  if (is_clicked) {
    handle_x += x - last_point.x();
    handle_rect.setXYWH(handle_x, 0, handle_width, contentHeight());
    last_point.set(x, y);
    markLayoutDirty();
  }
}

void Splitter::onMouseLeftPressed(const float x, const float y) {
  yuri::info("move: {}", x - last_point.x());
  if (handle_rect.contains(x, y)) {
    last_point.set(x, y);
    is_clicked = true;
  }
}

void Splitter::onMouseLeftReleased(const float x, const float y) {
  is_clicked = false;
}

} // namespace ui::widgets
