# 像素之旅 · Pixel Journey

从零构建的 C++23 UI 框架，基于 Vulkan + Skia 实现高性能渲染，目标是打造一个可复用的桌面 UI 系统。

> 项目起因：QWidget 绘制复杂界面性能不足，QML 又不够灵活，于是决定自己从底层开始造一个。

## 技术栈

| 项 | 技术 |
|---|---|
| 语言 | C++23 / C++ Modules (.cppm) |
| 图形 API | Vulkan (vulkan-hpp) |
| 2D 渲染 | Skia |
| 窗口管理 | GLFW |
| 构建系统 | CMake ≥ 4.0.0 + Ninja |
| 编译器 | MSVC ≥ 19.38 / GCC ≥ 15.0 / Clang ≥ 19.0 |

## 公共模块

所有对外模块统一使用 `yuri.` 前缀；应用代码通常只需要导入聚合模块：

```cpp
import yuri.core;          // 核心设施、日志、工具与性能分析
import yuri.configuration; // 通用与 Vulkan 配置
import yuri.glfw;          // 窗口与 GLFW 集成
import yuri.skia;          // Skia API 与资源
import yuri.vulkan;        // Vulkan API、上下文与细节封装
import yuri.ui;            // UI 聚合入口
import yuri.log;           // 仅使用日志时的轻量入口
```

各目录下的 `yuri.*` 子模块和 `:partition` 仅用于库内部组织；应用应优先使用上述聚合模块。

## 项目结构

```
pixel-journey/
├── cmake/                          # CMake 构建配置
│   ├── compiler_options.cmake      # 编译器选项
│   ├── cxx_modules.cmake           # C++23 模块支持
│   ├── dependencies.cmake          # 第三方依赖 (Skia, GLFW, Vulkan, FreeType)
│   ├── source_files.cmake          # 源文件收集
│   └── toolchain.cmake             # 工具链配置 (vcpkg)
├── docs/                           # 文档
├── script/                         # 构建脚本
├── src/                            # 框架源码
│   ├── configuration/              # 配置模块 (通用配置、Vulkan 配置)
│   ├── core/                       # 核心基础设施
│   │   ├── profiling/              #   性能分析 (FPS 计数、帧时钟)
│   │   ├── utils/                  #   工具类 (UUID 生成等)
│   │   ├── signal.cppm             #   信号槽机制
│   │   ├── type_traits.cppm        #   类型特征
│   │   └── yuri_log.cppm           #   日志系统
│   ├── glfw/                       # GLFW 窗口封装
│   ├── skia/                       # Skia 渲染层封装
│   │   ├── api/                    #   Skia API 包装
│   │   └── resource/               #   资源管理 (颜色、字体)
│   ├── vulkan/                     # Vulkan 图形 API 封装
│   │   ├── api/                    #   Vulkan API 包装
│   │   └── detail/                 #   实现细节 (实例创建、交换链)
│   └── ui/                         # UI 框架 (核心重点)
│       ├── algorithm/              #   算法工具 (线性插值 lerp)
│       ├── animation/              #   动画系统
│       │   ├── core.cppm           #     IAnimation 接口
│       │   ├── linear_animation    #     线性值动画
│       │   └── animation_manager   #     动画管理器 (单例)
│       ├── layout/                 #   布局系统
│       │   ├── core/               #     布局基类 Layout<T>
│       │   ├── layouts/            #     具体布局 (HBox / VBox)
│       │   └── types/              #     布局类型 (Alignment, Insets, SizeConstraints)
│       ├── render/                 #   渲染节点
│       │   ├── render_base         #     渲染基类 RenderNode
│       │   ├── render_background   #     背景填充 (支持圆角)
│       │   ├── render_border       #     边框描边
│       │   ├── render_text         #     文本渲染
│       │   └── render_svg          #     SVG 渲染 (光栅化到 GPU 纹理)
│       ├── widgets/                #   控件库
│       │   ├── core/               #     Widget 基类 / WindowBase
│       │   └── components/         #     Box, Button, Text, Splitter
│       └── ui.cppm                 #   UI 模块入口 (聚合导出)
├── test/                           # 测试与示例
├── CMakeLists.txt                  # 主构建配置
├── LICENSE                         # MIT 许可证
└── README.md
```

## UI 框架架构

UI 框架采用分层设计，从底向上依次为：

```
┌─────────────────────────────────────────┐
│  Widgets   │  Box, Button, Text, Splitter │  ← 可交互控件
├─────────────────────────────────────────┤
│  Layout    │  HBox, VBox, Constraints     │  ← 空间布局
├─────────────────────────────────────────┤
│  Render    │  Background, Border, Text    │  ← 绘制节点
├─────────────────────────────────────────┤
│  Animation │  LinearAnimation, Manager    │  ← 动画系统
├─────────────────────────────────────────┤
│  Algorithm │  lerp (数值/颜色插值)        │  ← 数学工具
└─────────────────────────────────────────┘
```

- **数据流**: 用户输入 → Widget 事件处理 → Layout 标记脏 → 布局更新 → RenderNode → Skia 渲染
- **动画流**: AnimationManager → lerp 插值 → Widget 属性更新 → 布局/渲染刷新

## 快速开始

```bash
# 克隆项目
git clone https://github.com/love-yuri/pixel-journey.git
cd pixel-journey

# 编译 Skia (首次需要)
python script/build.py

# 构建项目
mkdir build && cd build
cmake .. -G Ninja
cmake --build .

# 运行测试
ctest
```

> **Windows**: 使用 Clang 构建需链接 libc++，使用 MSVC 可直接在 VS 开发者终端中构建。

## 项目特性

- C++23 全标准特性，含 Modules (.cppm)
- 基于 Vulkan 的 GPU 加速渲染
- Skia 2D 图形引擎，支持 SVG / 文本 / 几何绘制
- 完整的 Widget 事件系统（鼠标、信号槽）
- 灵活的布局系统（HBox / VBox / Splitter / 约束）
- 帧动画系统与全局动画管理器
- 现代 CMake 构建，支持跨平台编译

## 许可证

[MIT](LICENSE) · Copyright 2025 夏亦寒
