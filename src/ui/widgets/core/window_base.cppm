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
  virtual void setCursor(CursorType type) = 0;
};


}