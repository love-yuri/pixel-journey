//
// Created by love-yuri on 2026/1/29.
//
module;
#if defined(_WIN32)
// ReSharper disable once CppUnusedIncludeDirective
#include "include/private/base/SkFloatingPoint.h"
#endif
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

class SplitterLayout;

export namespace ui::widgets {

enum class SplitterOrientation {
  Horizontal,
  Vertical,
};

class SplitterHandle {
public:
  static constexpr auto default_handle_width = 1;
  static constexpr auto default_handle_x = 200;

  explicit SplitterHandle(
    Widget &widget,
    const SplitterOrientation orientation = SplitterOrientation::Horizontal) noexcept :
    orientation_(orientation), widget_(widget) {
    update();
  }

  void update() {
    if (orientation_ == SplitterOrientation::Horizontal) {
      handle_rect.setXYWH(default_handle_x, 0, default_handle_width, widget_.contentHeight());
    } else {
      handle_rect.setXYWH(0, default_handle_x, widget_.contentWidth(), default_handle_width);
    }
  }

  void move(const float x, const float y) {
    handle_rect.setXYWH(x, y, default_handle_width, widget_.contentHeight());
  }

  void move(const float x) {
    handle_rect.setXYWH(handle_rect.x() + x, 0, default_handle_width, widget_.contentHeight());
  }

  void draw(SkCanvas *canvas) const {
    canvas->drawRect(handle_rect, sk_paint);
  }

  void setHandleWidth(const float width) {
    if (orientation_ == SplitterOrientation::Horizontal) {
      const float center_x = handle_rect.centerX();
      handle_rect.setXYWH(center_x - width / 2.f, 0, width, widget_.contentHeight());
    } else {
      const float center_y = handle_rect.centerY();
      handle_rect.setXYWH(0, center_y - width / 2.f, widget_.contentWidth(), width);
    }
  }

  [[nodiscard]] bool contains(const float x, const float y) const {
    return x > handle_rect.x() - 5 && x < handle_rect.right() + 5;
  }

  [[nodiscard]] const SkRect &bounds() const {
    return handle_rect;
  }

protected:
  SkRect handle_rect{};
  SplitterOrientation orientation_;
  Widget &widget_;
  SkPaint sk_paint = SkPaintBuilder().setColor(skia_colors::gray).build();
};

class Splitter : public Widget {
  friend class SplitterLayout;

public:
  explicit Splitter(Widget *parent);

  [[nodiscard]] const std::vector<SplitterHandle> &handles() const noexcept {
    return handles_;
  }

  void render(SkCanvas *canvas) override;
  void addWidget(Widget *widget) override;
  void layoutChildren() override;

protected:
  void onMouseMove(float x, float y) override;
  void onMouseLeftPressed(float x, float y) override;
  void onMouseLeftReleased(float x, float y) override;

private:
  std::vector<SplitterHandle> handles_;
  SkPoint last_point{};
  bool is_clicked = false;
  SplitterOrientation orientation_{};
  SplitterHandle *selected_handle = nullptr;
};

/**
 * Splitter布局策略 - 根据handle位置划分子控件空间
 * 仅在Splitter内部使用
 */
class SplitterLayout : public Layout<Widget> {
public:
  explicit SplitterLayout(Widget *widget) : Layout(widget) {
  }

  void apply() const override {
    const auto &children = widget_->children();
    const auto child_count = children.size();
    if (child_count == 0) return;

    const auto *splitter = static_cast<const Splitter *>(widget_);
    const auto &handles = splitter->handles();
    const auto h = handles.size();
    const auto content_w = widget_->contentWidth();
    const auto content_h = widget_->contentHeight();

    if (h == 0) {
      const float w = content_w / static_cast<float>(child_count);
      float x = 0;
      for (const auto &child : children) {
        setGeometry(child, x, 0, w, content_h);
        x += w;
      }
      return;
    }

    setGeometry(children[0], 0, 0, handles[0].bounds().x(), content_h);

    for (std::size_t i = 1; i < h; ++i) {
      const float left = handles[i - 1].bounds().right();
      const float right = handles[i].bounds().x();
      setGeometry(children[i], left, 0, right - left, content_h);
    }

    setGeometry(children[h], handles[h - 1].bounds().right(), 0,
                content_w - handles[h - 1].bounds().right(), content_h);
  }
};

Splitter::Splitter(Widget *parent) : Widget(parent) {
  setLayout<SplitterLayout>();
}

void Splitter::render(SkCanvas *canvas) {
  canvas->save();

  canvas->translate(x_, y_);
  paint(canvas);

  canvas->translate(padding_.left, padding_.top);

  for (const auto &child : children_) {
    if (child->visible()) {
      child->render(canvas);
    }
  }

  for (auto &handle : handles_) {
    handle.draw(canvas);
  }

  canvas->restore();
}

void Splitter::addWidget(Widget *widget) {
  Widget::addWidget(widget);
  if (children_.size() >= 2) {
    handles_.emplace_back(*this, orientation_);
  }
  markLayoutDirty();
}

void Splitter::layoutChildren() {
  if (layout_) {
    layout_->apply();
  }

  for (const auto &child : children_) {
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
  if (selected_handle) {
    selected_handle->setHandleWidth(SplitterHandle::default_handle_width);
    selected_handle = nullptr;
  }
  window()->setCursor(CursorType::Arrow);
}

} // namespace ui::widgets
