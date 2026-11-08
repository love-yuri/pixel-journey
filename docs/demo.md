# 分栏示例

示例入口为 `test/demo.cpp`，使用 yuri_lib 的 Window、Splitter、Widget 和 Button 搭建左右分栏界面。左侧最小宽度为 300，包含一个“首页”按钮；右侧为预留内容的空白区域。分隔线支持拖动，按钮尚未绑定业务操作。

## 构建与运行

在项目根目录执行（依赖配置见 README）：

```bash
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DBUILD_TESTING=ON
cmake --build build --target demo
./build/test/demo
```

Windows 使用对应工具链构建后运行生成的 `demo.exe`；多配置生成器的程序位于对应配置目录中。

示例需要桌面显示服务与支持 Vulkan 的图形设备，字体使用库默认配置。它是持续运行的交互程序，通过关闭窗口退出，因此不注册为 CTest 自动测试。
