#!/bin/bash
set -euo pipefail

# ====================================
# Skia 安装 & 构建脚本
# ====================================

SKIA_ROOT="${SKIA_ROOT:-$HOME/github/skia}"
SKIA_REPO="https://skia.googlesource.com/skia.git"
OUT_DIR="$SKIA_ROOT/out/Shared"

# ====================================
# 1. 克隆 Skia (如果尚未克隆)
# ====================================
if [ ! -d "$SKIA_ROOT" ]; then
    echo "==> 克隆 Skia 仓库..."
    mkdir -p "$(dirname "$SKIA_ROOT")"
    git clone "$SKIA_REPO" "$SKIA_ROOT"
else
    echo "==> Skia 目录已存在，跳过克隆"
fi

cd "$SKIA_ROOT"

# ====================================
# 2. 同步第三方依赖
# ====================================
echo "==> 同步第三方依赖..."
python3 tools/git-sync-deps

# ====================================
# 3. 调用 Python 构建脚本
# ====================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo "==> 开始构建 Skia..."

python3 "$SCRIPT_DIR/build.py"

echo "==> Skia 安装构建完成！"
echo "    输出目录: $OUT_DIR"
