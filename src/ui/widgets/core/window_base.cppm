//
// Created by yuri on 2026/1/29.
//

export module ui.widgets:window_base;

import glfw.api;

using namespace glfw;

export namespace ui::widgets {

class WindowBase {
public:
  virtual ~WindowBase() = default;
  void setCursor(CursorType type);

  /**
   * 获取当前鼠标类型
   */
  CursorType cursor() const noexcept {
    return cursor_;
  }

protected:
  // 子类重写设置鼠标指针
  virtual void doSetCursor(CursorType type) = 0;

  CursorType cursor_ = CursorType::Arrow; // 当前cursor
};

void WindowBase::setCursor(const CursorType type) {
  if (type == cursor_) {
    return;
  }
  cursor_ = type;
  doSetCursor(type);
}

} // namespace ui::widgets