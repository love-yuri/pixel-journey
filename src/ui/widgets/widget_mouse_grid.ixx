//
// Created by yuri on 2026/1/15.
//

export module ui.widgets:widget_mouse_grid;

import skia.api;
import utils.math;
import yuri_log;
import std;

using namespace skia;

export namespace ui::widgets {

struct Widget {
  SkRect rect;
};

/**
 * 鼠标操作网格窗口
 */
class WidgetMouseGrid {
  int width_;   // 窗口宽度
  int height_;  // 窗口高度
  int rows_;    // 网格行数
  int columns_; // 网格列数
  std::vector<std::vector<std::vector<SkRect*>>> widgets_;

public:
  static constexpr int cell_width = 8;
  WidgetMouseGrid(int width, int height);

  /**
   * 添加控件
   * @param widget 控件指针
   */
  void add_widget(SkRect* widget);

  void check(float x, float y);
};

WidgetMouseGrid::WidgetMouseGrid(const int width, const int height) :
  width_(width),
  height_(height),
  rows_(math::ceil(width, cell_width)),
  columns_(math::ceil(height, cell_width)) {
  widgets_ = std::vector(rows_, std::vector(columns_, std::vector<SkRect*>()));
}

void WidgetMouseGrid::add_widget(SkRect *rectr) {
  const auto rect = *rectr;
  const auto left_x = static_cast<int>(rect.left() / cell_width);
  const auto right_x = static_cast<int>(rect.right() / cell_width);
  const auto top_y = static_cast<int>(rect.top() / cell_width);
  const auto bottom_y = static_cast<int>(rect.bottom() / cell_width);

  yuri::info("w: {}, h: {}, cols: {}, rows: {}", width_, height_, columns_, rows_);

  // 添加 Widget
  for (int i = left_x; i <= right_x; ++i) {
    for (int j = top_y; j <= bottom_y; ++j) {
      yuri::info("add: {} {}", i, j);
      widgets_[i][j].emplace_back(rectr);
    }
  }
}

void WidgetMouseGrid::check(float x, float y) {
  int xx = x / cell_width;
  int yy = y / cell_width;
  if (widgets_[xx][yy].size() > 0) {
    yuri::error("check: {}", widgets_[xx][yy].size());
  } else {
    yuri::info("没找到");
  }
}

} // namespace ui::widgets