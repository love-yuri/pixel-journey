import yuri.ui;
import yuri.glfw;
import yuri.skia;
import yuri.log;

using namespace ui::widgets;
using namespace skia;
using namespace glfw;
using namespace ui::layout;

static void create_left_widget(Splitter* splitter) {
  const auto widget = new Widget(splitter);
  widget->setLayout<VBoxLayout<Widget>>();
  const auto button = new Button("首页", widget);
  button->setMaxHeight(40);
}

static void create_right_widget(Splitter* splitter) {
  const auto widget = new Widget(splitter);
  widget->setLayout<VBoxLayout<Widget>>();
}

int main() {
  Window window(1080, 720, "yuri");
  window.setLayout<HBoxLayout<Widget>>();

  const auto splitter = new Splitter(&window);
  splitter->setMinLeftWidth(300);

  // 创建左侧界面
  create_left_widget(splitter);

  // 创建右侧界面
  create_right_widget(splitter);

  window.show();
}
