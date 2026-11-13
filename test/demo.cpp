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
  const auto layout = new VBoxLayout();

  widget->set_layout(layout);
  const auto button = new Button("首页", widget);
  button->set_max_width(200);
  button->set_horizontal_alignment(HorizontalAlignment::Center);
  button->set_max_height(40);
  button->set_margin(10);
}

static void create_right_widget(Splitter* splitter) {
  const auto widget = new Widget(splitter);
  const auto layout = new VBoxLayout();

  widget->set_layout(layout);
}

int main() {
  Window window(1080, 720, "yuri");
  window.set_layout(new HBoxLayout());

  const auto splitter = new Splitter(&window);
  splitter->set_min_left_width(300);

  // 创建左侧界面
  create_left_widget(splitter);

  // 创建右侧界面
  create_right_widget(splitter);

  window.show();
}
