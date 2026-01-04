# 像素之旅 · Pixel Journey

一个从零开始的图形之旅。  
记录我对 Vulkan、渲染管线以及底层图形系统的探索。

> 因使用qwidget绘制复杂界面性能不好，qml又太狗屎所以打算自己重绘
> 所以这个既是我学习vulkan和skia的仓库，也是我qq-music重构版本的开始
> 使用vulkan + skia + c++23全新打造，别的不说反正很新

## 技术栈

- C++23 / Modules
- Vulkan (vulkan-hpp)
- GLFW / Wayland
- CMake + Ninja

## 项目要求

### 最低版本要求
> 需要安装libc++ libc++abi vulkan-headers glfw vulkan-validation-layers clang

- **CMake**: >= 4.0.0
    - 推荐版本: 4.0.2 或 4.2.0 (std标志支持)
- **C++标准**: C++23
- **编译器**:
    - MSVC >= 19.38 (Visual Studio 2022 17.8+)
    - GCC >= 15.0
    - Clang >= 19.0

> ⚠️ **注意**: C++ Modules 支持需要较新的编译器版本，请确保编译器完全支持C++23模块特性。

### Windows
windows下使用clang构建请链接libc++, 使用msvc可以直接进入vs终端构建

## 目录结构

```
modern-cpp/
├── cmake/                      # CMake 配置文件
├── script/                     # 脚本目录
├── src/                        # 源代码目录
│   └── utils/                  # 工具模块
├── test/                       # 测试目录
├── CMakeLists.txt              # 主CMake配置
```

## 快速开始

```bash

# 克隆项目
git clone https://github.com/love-yuri/modern-cpp.git
cd modern-cpp
mkdir build
cd build
cmake .. -G Ninja # 只能使用Ninja构建
cmake --build .   # 生成
ctest             # 启动测试，也可以单独运行`test`目录下的测试
```

## 项目特性

- ✅ 完全使用 C++23 标准
- ✅ 基于 C++ Modules (.ixx) 构建
- ✅ 编译时反射实现
- ✅ 现代化的 CMake 配置
- ✅ 模块化的项目结构
- ✅ 完整的测试支持

## 许可证

详见 [LICENSE](LICENSE) 文件。
