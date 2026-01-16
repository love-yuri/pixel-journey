//
// Created by yuri on 2026/1/15.
//

export module ui.widgets:hit_test_grid;

import skia.api;
import utils.math;
import yuri_log;
import std;
import :base;

using namespace skia;

using WidgetList = std::vector<ui::widgets::Widget*>;
using GridRow  = std::vector<WidgetList>;
using WidgetGrid = std::vector<GridRow>;

export namespace ui::widgets {

/**
 * 鼠标操作网格窗口
 */
class HitTestGrid {
  int width_;   // 窗口宽度
  int height_;  // 窗口高度
  int rows_;    // 网格行数
  int columns_; // 网格列数
  WidgetGrid cellWidgets_; // 控件网格

public:
  static constexpr int kCellWidth = 8;
  HitTestGrid(int width, int height);

  /**
   * 添加控件
   * @param widget 控件指针
   */
  void addWidget(Widget* widget);

  void check(float x, float y);
};

HitTestGrid::HitTestGrid(const int width, const int height) :
  width_(width),
  height_(height),
  rows_(math::ceil(width, kCellWidth)),
  columns_(math::ceil(height, kCellWidth)) {
  cellWidgets_ = std::vector(rows_, std::vector(columns_, std::vector<Widget*>()));
}

void HitTestGrid::addWidget(Widget *widget) {
  const auto rect = widget->hitTestBounds();
  const auto left_x = static_cast<int>(rect.left() / kCellWidth);
  const auto right_x = static_cast<int>(rect.right() / kCellWidth);
  const auto top_y = static_cast<int>(rect.top() / kCellWidth);
  const auto bottom_y = static_cast<int>(rect.bottom() / kCellWidth);

  yuri::info("w: {}, h: {}, cols: {}, rows: {}", width_, height_, columns_, rows_);

  // 添加 Widget
  for (int row = left_x; row <= right_x; ++row) {
    for (int col = top_y; col <= bottom_y; ++col) {
      yuri::info("add: {} {}", row, col);
      cellWidgets_[row][col].emplace_back(widget);
    }
  }
}

void HitTestGrid::check(float x, float y) {
  int row = x / kCellWidth;
  int col = y / kCellWidth;
  if (cellWidgets_[row][col].size() > 0) {
    yuri::error("check: {}", cellWidgets_[row][col].size());
  } else {
    yuri::info("没找到");
  }
}

} // namespace ui::widgets