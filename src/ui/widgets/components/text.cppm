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
  explicit Text(const std::string_view text, Widget* parent): Widget(parent), renderText(text, &self_box) {

  }

  void paint(SkCanvas *canvas) override {
    renderText.render(canvas);
  }

  void layoutChildren() override {
    renderText.reCalculate();
  }
};


}