module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
export module glfw.api;

import std;
import yuri_log;

export namespace glfw {

using ::GLFWwindow;
using ::glfwCreateWindowSurface;
using ::glfwGetFramebufferSize;
using ::glfwHideWindow;
using ::glfwShowWindow;
using ::glfwWindowShouldClose;
using ::glfwPollEvents;
using ::glfwSetFramebufferSizeCallback;
using ::glfwSetCursorPosCallback;
using ::glfwSetMouseButtonCallback;
using ::glfwSetCursorEnterCallback;
using ::glfwSetWindowUserPointer;
using ::glfwGetWindowUserPointer;
using ::glfwPollEvents;
using ::glfwMakeContextCurrent;
using ::glfwGetCursorPos;
using ::glfwSetWindowPos;

constexpr auto left_mouse_button = GLFW_MOUSE_BUTTON_LEFT;
constexpr auto right_mouse_button = GLFW_MOUSE_BUTTON_RIGHT;
constexpr auto button_pressed = GLFW_PRESS;
constexpr auto button_released = GLFW_RELEASE;

class Application final {
public:
  explicit Application() {
    if (glfwInit() == GLFW_FALSE) {
      throw std::runtime_error("glfw: 初始化失败!");
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  }

  ~Application() {
    glfwTerminate();
  }
};

/**
 * 获取vulkan所需扩展
 * @return 扩展合集
 */
std::vector<const char*> get_vk_extensions() {
  uint32_t extCount = 0;
  const char **extensions = glfwGetRequiredInstanceExtensions(&extCount);
  return std::vector<const char*> { extensions, extensions + extCount };
}

/**
 * 创建窗口
 * 使用 const std::string& 避免字符串越界
 * @param width 窗口宽度
 * @param height 窗口高度
 * @param title 窗口标题
 * @return 窗口指针
 */
GLFWwindow* create_window(const int width, const int height, const std::string& title) {
  return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

/**
 * 销毁窗口指针
 * @param window 窗口指针
 */
void destroy_window(GLFWwindow* window) {
  glfwDestroyWindow(window);
}

/**
 * 获取窗口的帧buffer大小
 * @return <width, height>
 */
std::tuple<std::uint32_t, std::uint32_t> get_buffer_size(GLFWwindow* window) {
  int height, width;
  glfwGetFramebufferSize(window, &width, &height);
  return {
    static_cast<std::uint32_t>(width),
    static_cast<std::uint32_t>(height)
  };
}

/**
 * 获取屏幕尺寸
 * @return <width, height>
 */
std::tuple<int, int> getPrimaryMonitorSize() {
  GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

  return { mode->width, mode->height };
}

}
