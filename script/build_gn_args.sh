# Wayland + Vulkan + Release + 系统字体
bin/gn gen out/Shared --args='
  is_official_build=true        # Release 官方构建
  is_debug=false                # 非 Debug
  skia_use_vulkan=true          # Vulkan 后端
  skia_use_gl=false             # 不用 OpenGL
  skia_use_fontconfig=true      # 系统字体发现
  skia_use_freetype=true        # 字体渲染
  cc="clang"                    # 使用 clang 作为 C 编译器
  cxx="clang++"                 # 使用 clang++ 作为 C++ 编译器
'