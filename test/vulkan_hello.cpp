#include <random>
import std;
import vulkan;
import glfw;

int main() {
  glfw::glfw_window gw = {200, 200, "yuri"};
  gw.show();

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution dis(0.0f, 1.0f);

  while (!gw.should_close()) {
    const auto frame = gw.acquire_next_frame();
    const auto cmd = frame->begin_frame();

    vk::ImageSubresourceRange range{
      vk::ImageAspectFlagBits::eColor, // 操作image，这里只能是eColor
      0, // 操作第 0 层mip
      1, // 仅操作 1 层mip
      0, // 从第0个layers开始操作
      1, // 仅操作1个layer
    };

    // UNDEFINED -> TRANSFER_DST
    cmd->pipelineBarrier(
      vk::PipelineStageFlagBits::eTopOfPipe, // 从管线从开始
      vk::PipelineStageFlagBits::eTransfer, // 到传输阶段
      {},
      nullptr,
      nullptr,
      vk::ImageMemoryBarrier{
        {}, // 源访问时 未做任何访问
        vk::AccessFlagBits::eTransferWrite, // 后续需要传输阶段可写
        vk::ImageLayout::eUndefined, // eUndefined -> eTransferDstOptimal
        vk::ImageLayout::eTransferDstOptimal,
        vk::QueueFamilyIgnored, // 不涉及队列操作
        vk::QueueFamilyIgnored,
        *frame->image,
        range,
      });

    // 生成随机颜色
    vk::ClearColorValue clearColor{
      std::array {
        dis(gen), // 红色分量
        dis(gen), // 绿色分量
        dis(gen), // 蓝色分量
        1.0f,
      },
    };

    cmd->clearColorImage(*frame->image, vk::ImageLayout::eTransferDstOptimal, clearColor, range);

    // TRANSFER_DST -> PRESENT
    cmd->pipelineBarrier(
      vk::PipelineStageFlagBits::eTransfer,    // 传输
      vk::PipelineStageFlagBits::eBottomOfPipe, // 渲染尾部
      {},
      nullptr,
      nullptr,
      vk::ImageMemoryBarrier{
        vk::AccessFlagBits::eTransferWrite,
        {},
        vk::ImageLayout::eTransferDstOptimal, // 传输阶段
        vk::ImageLayout::ePresentSrcKHR, // 提交阶段
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        *frame->image,
        range
      });

    frame->submit();
    frame->present();

    glfw::glfwPollEvents();
  }
}