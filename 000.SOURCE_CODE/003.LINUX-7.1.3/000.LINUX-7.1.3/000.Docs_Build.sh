#!/bin/bash

# ============================================================
# Linux 7.1.3 Documentation HTML Build Script
# ============================================================
# 内核文档基于 Sphinx 构建（make htmldocs）
# 编译产物输出到独立的 OUT_DIR 目录（不污染内核源码树）
# 首次运行会自动创建虚拟环境并安装依赖（Sphinx / alabaster / pyyaml）

# 内核源码目录（即脚本所在目录）
KERNEL_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$KERNEL_DIR"

# HTML 输出目录（独立于内核源码树，可按需修改）
OUT_DIR="$KERNEL_DIR/kernel-docs-output"

# Sphinx 虚拟环境路径（放在源码树外，避免污染内核目录）
VENV_DIR="$KERNEL_DIR/kernel-docs-output/.kernel-docs-venv"

# 网络代理（pip 安装依赖时使用；留空则直连）
PROXY="http://127.0.0.1:7890"
# pip 国内镜像（官方源安装失败时自动回退使用）
PIP_MIRROR="https://pypi.tuna.tsinghua.edu.cn/simple"

# 并行编译线程数
JOBS=$(nproc)

# 可选：只编译某个子目录的文档，用法示例：
#   ./000.Docs_Build.sh admin-guide   （只编译 Documentation/admin-guide）
# 不带参数则编译全部文档
SPHINXDIRS_ARG=""
if [ -n "$1" ]; then
    SPHINXDIRS_ARG="SPHINXDIRS=$1"
fi

# ---------- 2. 准备 Sphinx 环境 ----------
find_sphinx() {
    # 返回可用的 sphinx-build 路径（系统优先，其次 venv）
    if command -v sphinx-build &>/dev/null; then
        command -v sphinx-build
    elif [ -x "$VENV_DIR/bin/sphinx-build" ]; then
        echo "$VENV_DIR/bin/sphinx-build"
    else
        echo ""
    fi
}

create_venv() {
    # 创建虚拟环境并确认 pip 就位（防止生成残缺目录）
    python3 -m venv "$VENV_DIR" && [ -x "$VENV_DIR/bin/pip" ]
}

install_deps() {   # $1 = 额外的 pip 参数（如 "-i <镜像>"，可为空）
    "$VENV_DIR/bin/pip" install --retries 2 --timeout 30 $1 \
        -r "$KERNEL_DIR/Documentation/sphinx/requirements.txt"
}

SPHINX_BIN=$(find_sphinx)

if [ -z "$SPHINX_BIN" ]; then
    echo "未检测到 sphinx-build，准备创建 Python 虚拟环境并安装依赖 ..."
    # 虚拟环境残缺（如上次创建被中断）时删除重建
    if [ -d "$VENV_DIR" ] && [ ! -x "$VENV_DIR/bin/pip" ]; then
        echo "发现残缺的虚拟环境，删除重建 ..."
        rm -rf "$VENV_DIR"
    fi
    create_venv || {
        echo "错误：创建虚拟环境失败（Ubuntu 需先安装：sudo apt install python3-venv）"
        exit 1
    }
    # 优先走国内镜像直连（经代理下载大文件易出现 sha256 校验失败）
    unset http_proxy https_proxy
    install_deps "-i $PIP_MIRROR" || {
        echo "镜像安装失败，回退官方源（走代理）..."
        export http_proxy="$PROXY" https_proxy="$PROXY"
        install_deps "" || {
            echo "错误：依赖安装失败，请检查网络/代理设置（脚本顶部 PROXY）"
            exit 1
        }
    }
    SPHINX_BIN=$(find_sphinx)
fi

echo "使用 sphinx-build: $SPHINX_BIN ($("$SPHINX_BIN" --version 2>/dev/null | head -1))"

# 通过 SPHINXBUILD 变量传给内核 Documentation/Makefile（它默认只在 PATH 里找）
SPHINXBUILD_ARG="SPHINXBUILD=$SPHINX_BIN"

# ---------- 3. 编译 HTML 文档（输出到独立目录） ----------
# O= 指定 kbuild 输出树，文档生成在 $OUT_DIR/Documentation/output 下
make O="$OUT_DIR" htmldocs $SPHINXDIRS_ARG $SPHINXBUILD_ARG -j"$JOBS"
BUILD_RET=$?

# ---------- 4. 结果提示 ----------
HTML_OUT="$OUT_DIR/Documentation/output"
# 全量编译入口页在 output 根；SPHINXDIRS 模式下入口页在子目录里
ENTRY="$HTML_OUT/index.html"
if [ -n "$1" ]; then
    FIRST_DIR=${1%%,*}; FIRST_DIR=${FIRST_DIR%% *}   # 取第一个子目录名
    [ -f "$HTML_OUT/$FIRST_DIR/index.html" ] && ENTRY="$HTML_OUT/$FIRST_DIR/index.html"
fi

if [ $BUILD_RET -eq 0 ] && [ -f "$ENTRY" ]; then
    echo "============================================================"
    echo "文档编译完成：$ENTRY"
    echo "浏览器直接打开，或本地起服务预览："
    echo "  python3 -m http.server 8080 -d $HTML_OUT"
    echo "  然后访问 http://localhost:8080"
    echo "清理文档编译产物：make O=$OUT_DIR cleandocs"
    echo "============================================================"
elif [ $BUILD_RET -eq 0 ]; then
    echo "make 返回成功，但未找到入口页 $ENTRY，请检查输出目录"
    exit 1
else
    echo "编译失败（返回码 $BUILD_RET），请检查上方错误输出"
    exit $BUILD_RET
fi
