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

// 布局方式
enum class SplitterOrientation {
  Horizontal,
  Vertical,
};

class SplitterHandle {
public:
  static constexpr auto default_handle_width = 1; // 默认handle宽度
  static constexpr auto default_handle_x = 200;   // 默认handle起步x
  explicit SplitterHandle(Widget &widget, const SplitterOrientation orientation = SplitterOrientation::Horizontal) noexcept
    : orientation_(orientation),
      widget_(widget) {

    update();
  }

  // 更新布局
  void update() {
    if (orientation_ == SplitterOrientation::Horizontal) {
      handle_rect.setXYWH(default_handle_x, 0, default_handle_width, widget_.contentHeight());
    } else {
      handle_rect.setXYWH(0, default_handle_x, widget_.contentWidth(), default_handle_width);
    }
  }

  // 更新位置
  void move(const float x, const float y) {
    handle_rect.setXYWH(x, y, default_handle_width, widget_.contentHeight());
  }

  // 更新位置
  void move(const float x) {
    handle_rect.setXYWH(handle_rect.x() + x, 0, default_handle_width, widget_.contentHeight());
  }

  // 绘制
  void draw(SkCanvas *canvas) const {
    canvas->drawRect(handle_rect, sk_paint);
  }

  // 设置handle的宽度
  void setHandleWidth(const float width) {
    if (orientation_ == SplitterOrientation::Horizontal) {
      const float center_x = handle_rect.centerX();
      handle_rect.setXYWH(center_x - width / 2.f, 0, width, widget_.contentHeight());
    } else {
      const float center_y = handle_rect.centerY();
      handle_rect.setXYWH(0, center_y - width / 2.f, widget_.contentWidth(), width);
    }
  }

  // 判断鼠标是否在矩形内
  [[nodiscard]] bool contains(const float x, const float y) const {
    return x > handle_rect.x() - 5 && x < handle_rect.right() + 5;
  }

  // 获取矩形边界
  [[nodiscard]] const SkRect &bounds() const {
    return handle_rect;
  }

protected:
  SkRect handle_rect{};             // 中间拖动条
  SplitterOrientation orientation_; // 排布方式
  Widget &widget_;                  // 父控件
  SkPaint sk_paint = SkPaintBuilder()
    .setColor(skia_colors::gray)
    .build();
};

class Splitter : public Widget {

public:
  explicit Splitter(Widget *parent) : Widget(parent) {
    has_layout = true;
  }

protected:
  void render(SkCanvas *canvas) override;
  void addWidget(Widget *widget) override;
  void layoutChildren() override;
  void onMouseMove(float x, float y) override;
  void onMouseLeftPressed(float x, float y) override;
  void onMouseLeftReleased(float x, float y) override;

private:
  std::vector<SplitterHandle> handles_; // 间隔条
  SkPoint last_point{};                 // 上次点击点
  bool is_clicked = false;              // 是否被点击
  SplitterOrientation orientation_{};   // 排布方式
  std::size_t last_layout_handle_size = 0;
  SplitterHandle* selected_handle = nullptr;
};

void Splitter::render(SkCanvas *canvas) {
  canvas->save();
  // 绘制自身
  paint(canvas);

  // 变换后绘制children
  canvas->translate(self_box.x() + padding_.left, self_box.y() + padding_.top);

  // 绘制child
  for (const auto child : children_) {
    child->render(canvas);
  }

  // 绘制间隔条
  for (auto& handle : handles_) {
    handle.draw(canvas);
  }

  canvas->restore();
}

void Splitter::addWidget(Widget *widget) {
  Widget::addWidget(widget);
  yuri::info("add widget");
  if (children_.size() < 2) {
    return;
  }
  handles_.emplace_back(*this, orientation_);
  markLayoutDirty();
}

void Splitter::layoutChildren() {
  const auto handle_size = handles_.size();
  const auto w = (contentWidth() - handle_size * SplitterHandle::default_handle_width) / (handle_size + 1);
  if (last_layout_handle_size != handle_size) {
    for (auto i = 0u; i < handles_.size(); ++i) {
      auto &handle = handles_[i];
      handle.move((i + 1) * w + i * SplitterHandle::default_handle_width, 0);
    }
    last_layout_handle_size = handle_size;
  }

  for (auto i = 0u; i < children_.size(); ++i) {
    if (i == 0) {
      const auto &handle = handles_[0];
      const auto child = children_[i];
      setGeometry(child, 0, 0, handle.bounds().x(), contentHeight());
    } else {
      const auto &handle = handles_[i - 1];
      const auto child = children_[i];
      setGeometry(child, handle.bounds().right(), 0, contentWidth() - handle.bounds().right(), contentHeight());
    }
  }

  // 更新子控件布局
  for (const auto child : this->children_) {
    child->updateLayout();
  }
}

void Splitter::onMouseMove(const float x, const float y) {
  if (!is_clicked) {
    auto has_find = false;
    for (auto &handle : handles_) {
      if (handle.contains(x, y)) {
        has_find = true;
        handle.setHandleWidth(5);
        break;
      }

      handle.setHandleWidth(SplitterHandle::default_handle_width);
    }

    if (has_find) {
      window()->setCursor(CursorType::HResize);
    } else {
      window()->setCursor(CursorType::Arrow);
    }
  }

  if (is_clicked && selected_handle) {
    selected_handle->move(x - last_point.x());
    selected_handle->setHandleWidth(5);
    last_point.set(x, y);
    markLayoutDirty();
  }
}

void Splitter::onMouseLeftPressed(const float x, const float y) {
  for (auto &handle : handles_) {
    if (handle.contains(x, y)) {
      selected_handle = &handle;
      last_point.set(x, y);
      window()->setCursor(CursorType::HResize);
      is_clicked = true;
      break;
    }
  }
}

void Splitter::onMouseLeftReleased(const float x, const float y) {
  is_clicked = false;
  selected_handle->setHandleWidth(SplitterHandle::default_handle_width);
  selected_handle = nullptr;
  window()->setCursor(CursorType::Arrow);
}

} // namespace ui::widgets
