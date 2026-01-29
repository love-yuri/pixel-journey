//
// Created by love-yuri on 2026/1/29.
//

export module ui.widgets:layout_widget;

import :base;
import ui.layout;

using namespace ui::layout;

export namespace ui::widgets {

/**
 * 支持布局的widget
 */
class LayoutWidget: public Widget {
  using Widget::Widget;
  std::unique_ptr<Layout<Widget>> layout_ = nullptr;  // 布局

public:
  void layoutChildren() override {
    // 重新布局
    if (layout_ != nullptr) {
      layout_->apply();
    }

    // 更新子控件布局
    for (const auto child : this->children_) {
      child->updateLayout();
    }
  }

  /**
   * 设置布局
   */
  template <typename LayoutType>
  void setLayout() {
    static_assert(std::is_base_of_v<Layout<Widget>, LayoutType>, "LayoutType must inherit from Layout<Widget>");
    layout_ = std::make_unique<LayoutType>(this);
    has_layout = true;
  }

  /**
   * 获取布局指针-不拥有所有权
   * @return 布局指针
   */
  Layout<Widget> *layout() const {
    return layout_.get();
  }

  /**
   * 移除布局
   */
  void removeLayout() {
    layout_ = nullptr;
    has_layout = false;
  }
};

}