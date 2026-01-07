//
// Created by yuri on 2026/1/6.
//

export module glfw.context;

import std;
import vulkan.api;
import glfw.api;

export struct glfw_context {
  glfw::GLFWwindow *window;
  vk::SurfaceKHR surface;
  vk::Format format;
  vk::ColorSpaceKHR colorSpace;
  vk::SurfaceCapabilitiesKHR capabilities;
  vk::SwapchainKHR swapchain;
  vk::Image image;
};
