export module glfw:window;

import glfw_api;
import std;

export namespace glfw {

class glfw_window {
  int m_width;
  int m_height;
  GLFWwindow *m_window{};
  std::string m_title;

public:
  glfw_window(const int width, const int height, const std::string_view title = "yuri") :
    m_width(width),
    m_height(height),
    m_title(title) {
    m_window = create_window(width, height, title);
  }

  virtual ~glfw_window() {
    destroy_window(m_window);
  }

  /**
   * 获取窗口宽度
   */
  [[nodiscard]] int width() const {
    return m_width;
  }

  /**
   * 获取窗口高度
   */
  [[nodiscard]] int height() const {
    return m_height;
  }
};

} // namespace glfw