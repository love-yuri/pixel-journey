//
// Created by love-yuri on 2026/1/16.
//

export module ui.widgets:base;

import skia.api;
import yuri_log;

using namespace skia;

export namespace ui::widgets {

class Widget {

public:
  virtual ~Widget() = default;

  /**
   * 命中检测边界
   * @return 一个rect
   */
  virtual SkRect hitTestBounds() const = 0;
};

}