//
// Created by love-yuri on 2026/1/27.
//

export module yuri.ui.widgets:text;

import :base;
import yuri.ui.render;

using namespace ui::render;

export namespace ui::widgets {

class Text: public Widget {
  RenderText render_text_; // 字体节点
public:
  explicit Text(const std::string_view text, Widget* parent): Widget(parent), render_text_(text) {

  }

  void paint(SkCanvas *canvas) override {
    render_text_.draw(canvas);
  }

  void layout_children() override {
    render_text_.update(local_bounds());
  }

  /**
   * 设置对齐方式
   */
  inline void set_alignment(const Alignment alignment) {
    render_text_.set_alignment(alignment);
  }

  /**
   * 设置字体大小
   * @param size 字体大小
   */
  inline void set_font_size(const float size) {
    render_text_.set_font_size(size);
  }

  /**
   * 设置字体颜色
   */
  inline void set_color(const SkColor color) {
    render_text_.set_color(color);
  }

  /**
   * 设置文本
   */
  inline void set_text(const std::string_view text) {
    render_text_.set_text(text);
  }

  /**
   * 设置文本和对齐方式
   */
  inline void set_text_and_alignment(const std::string_view text, const Alignment alignment) {
    render_text_.set_text_and_alignment(text, alignment);
  }

};


}
