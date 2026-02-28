//
// Created by love-yuri on 2026/1/27.
//

export module ui.widgets:text;

import :base;
import ui.render;

using namespace ui::render;

export namespace ui::widgets {

class Text: public Widget {
  RenderText renderText; // 字体节点
public:
  explicit Text(const std::string_view text, Widget* parent): Widget(parent), renderText(text) {

  }

  void paint(SkCanvas *canvas) override {
    renderText.render(canvas);
  }

  void layoutChildren() override {
    renderText.update(contentRect());
  }

  /**
   * 设置对齐方式
   */
  inline void setAlignment(const Alignment alignment) {
    renderText.setAlignment(alignment);
  }

  /**
   * 设置字体大小
   * @param size 字体大小
   */
  inline void setFontSize(const float size) {
    renderText.setFontSize(size);
  }

  /**
   * 设置字体颜色
   */
  inline void setColor(const SkColor color) {
    renderText.setColor(color);
  }

  /**
   * 设置文本
   */
  inline void setText(const std::string_view text) {
    renderText.setText(text);
  }

  /**
   * 设置文本和对齐方式
   */
  inline void setTextAndAlignment(const std::string_view text, const Alignment alignment) {
    renderText.setTextAndAlignment(text, alignment);
  }

};


}