#!/usr/bin/env python3
"""克隆、同步并构建 Skia，支持 Windows 和 Linux。"""

import os
from pathlib import Path
import subprocess
import sys


def main():
    """完成 Skia 源码获取、依赖同步和构建。"""
    script_dir = Path(__file__).resolve().parent  # 脚本所在目录
    skia_root = Path(os.environ.get(
        "SKIA_ROOT", "C:/skia" if os.name == "nt" else "~/github/skia"
    )).expanduser().resolve()  # 统一解析源码路径，避免工作目录变化影响相对路径
    out_dir = skia_root / "out" / "Shared"  # 构建输出目录
    env = os.environ.copy()  # 子进程环境
    env["SKIA_ROOT"] = str(skia_root)

    if not skia_root.is_dir():
        print("==> 克隆 Skia 仓库...", flush=True)
        skia_root.parent.mkdir(parents=True, exist_ok=True)
        subprocess.check_call(
            ["git", "clone", "https://skia.googlesource.com/skia.git", str(skia_root)],
            env=env,
        )
    else:
        print("==> Skia 目录已存在，跳过克隆", flush=True)

    print("==> 同步第三方依赖...", flush=True)
    subprocess.check_call([sys.executable, "tools/git-sync-deps"], cwd=skia_root, env=env)

    print("==> 开始构建 Skia...", flush=True)
    subprocess.check_call([sys.executable, str(script_dir / "build.py")], cwd=skia_root, env=env)

    print("==> Skia 安装构建完成！")
    print("    输出目录:", out_dir)


if __name__ == "__main__":
    main()
