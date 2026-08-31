//
// Created by yuri on 2026/1/29.
//

export module yuri.ui.widgets:window_base;

import yuri.glfw.api;
import yuri.skia.api;

using namespace glfw;

export namespace ui::widgets {

class WindowBase {
public:
  virtual ~WindowBase() = default;
  void setCursor(CursorType type);

  /**
   * 获取当前鼠标在窗口内的位置
   * @return 鼠标坐标
   */
  [[nodiscard]] virtual skia::SkPoint cursorPosition() const = 0;

  /**
   * 获取当前鼠标类型
   */
  [[nodiscard]] CursorType cursor() const noexcept {
    return cursor_;
  }

  // 子类重写设置鼠标指针
  virtual void doSetCursor(CursorType type) = 0;

protected:
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
