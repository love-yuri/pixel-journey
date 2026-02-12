import os
import subprocess

# -----------------------------
# 配置 Skia 根目录
# -----------------------------
SKIA_ROOT = r"D:\skia" if os.name == "nt" else "/home/yuri/github/skia"
OUT_DIR = os.path.join(SKIA_ROOT, "out", "Shared")

# GN 可执行文件
GN_EXE = os.path.join(SKIA_ROOT, "bin", "gn.exe" if os.name == "nt" else "gn")

# Ninja 可执行文件
NINJA_EXE = "ninja.exe" if os.name == "nt" else "ninja"

# -----------------------------
# 编译参数
# -----------------------------
# 公共参数
args = [
    "is_official_build=true",  # Release
    "is_debug=false",          # 非debug
    "is_component_build=false",# 编译静态库
    "skia_use_vulkan=true",    # 使用vulkan
    "skia_use_gl=false",       # 不使用opengl
    "skia_enable_svg=true",    # 使用SVG
    # "skia_enable_tools=true",  # 启用 tools"
    r'cc="clang"',             # 使用clang
    r'cxx="clang++"',          # 使用clang++
]

# 后端选择
if os.name == "nt":
    # Windows 特定
    args += [
        "skia_pdf_subset_harfbuzz=true",
        # 使用 skia 自带库
        "skia_use_system_expat=false",
        "skia_use_system_harfbuzz=false",
        "skia_use_system_icu=false",
        "skia_use_system_libjpeg_turbo=false",
        "skia_use_system_libpng=false",
        "skia_use_system_libwebp=false",
        "skia_use_system_zlib=false",
        r'clang_win="D:/LLVM"',
    ]
else:
    # Linux 特定
    args += [
        "skia_use_fontconfig=true",
        "skia_use_freetype=true",  # 使用ttf
    ]

# -----------------------------
# 调用 GN 生成 Ninja 构建文件
# -----------------------------
subprocess.check_call([GN_EXE, "gen", OUT_DIR, "--args=" + " ".join(args)], shell=False)

# -----------------------------
# 调用 Ninja 编译
# -----------------------------
subprocess.check_call([NINJA_EXE, "-C", OUT_DIR], shell=False)

print("Skia 编译完成！输出目录:", OUT_DIR)
