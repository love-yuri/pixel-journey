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

public:
  explicit Splitter(Widget *parent) : Widget(parent) {
    has_layout = true;
  }

protected:
  void render(SkCanvas *canvas) override;
  void addWidget(Widget *widget) override;
  void paint(SkCanvas *canvas) override;
  void layoutChildren() override;

private:
  // 布局方式
  enum SplitterOrientation {
    Vertical,
    Horizontal
  };

  SplitterOrientation orientation_ = Horizontal;
  SkPaint sk_paint = PaintDesc {
    .color = skia_colors::gray,
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

    // 绘制间隔条
    canvas->drawRect({ (width_ - 10) / 2, 0, (width_ - 10) / 2 + 10, contentHeight() }, sk_paint);

    // 绘制右侧控件
    children_[1]->render(canvas);
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
  yuri::info("size: {} , {}", width_, height_);
  if (!children_.empty()) {
    if (children_.size() > 1) {
      children_[0]->setGeometry(children_[0], 0, 0, (width_ - 10) / 2, height_);
      children_[1]->setGeometry(children_[1], (width_ - 10) / 2 + 10, 0, (width_ - 10) / 2, height_);
    } else {
      children_[0]->setGeometry(children_[0], content_box);
    }

    // children_[1]->has_layout = true;
  }

  // 更新子控件布局
  for (const auto child : this->children_) {
    child->updateLayout();
  }
}

} // namespace ui::widgets
