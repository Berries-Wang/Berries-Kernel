#!/bin/bash

# ============================================================
# Linux 7.1.3 Documentation HTML Build Script (macOS 版)
# ============================================================
# 与 Linux 版 000.Docs_Build.sh 逻辑一致（内核文档基于 Sphinx 构建），
# 差异点（macOS 特有）：
#   1) nproc 命令不存在        -> 改用 sysctl -n hw.ncpu
#   2) Apple 自带 make 是 GNU Make 3.81，而内核 Makefile 要求
#      GNU Make >= 4.0（Makefile 第 15 行会直接报错）
#      -> 优先查找 Homebrew 的 gmake（brew install make），
#         没有则在安装依赖之前就快速失败并给出提示
#   3) 系统自带 python3 是 Xcode CLT 的 3.9（偏旧）
#      -> 自动挑选 3.10+ 的解释器（python3.13/3.12/3.11/3.10）创建 venv
#   4) 编译完成后可直接用 macOS 的 open 命令打开入口页
#
# 首次运行会自动创建虚拟环境并安装依赖（Sphinx / alabaster / pyyaml）

# 内核源码目录（即脚本所在目录）
KERNEL_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$KERNEL_DIR"

# HTML 输出目录（独立于内核源码树，可按需修改）
OUT_DIR="$KERNEL_DIR/kernel-docs-output"

# Sphinx 虚拟环境路径（放在输出目录下，避免污染内核目录）
VENV_DIR="$KERNEL_DIR/kernel-docs-output/.kernel-docs-venv"

# 网络代理（pip 安装依赖时使用；留空则直连）
PROXY="http://127.0.0.1:7890"
# pip 国内镜像（官方源安装失败时自动回退使用）
PIP_MIRROR="https://pypi.tuna.tsinghua.edu.cn/simple"

# 并行编译线程数（macOS 没有 nproc，用 sysctl）
JOBS=$(sysctl -n hw.ncpu)

# 可选：只编译某个子目录的文档，用法示例：
#   ./000.Docs_Build_MacOS.sh admin-guide   （只编译 Documentation/admin-guide）
# 不带参数则编译全部文档
SPHINXDIRS_ARG=""
if [ -n "$1" ]; then
    SPHINXDIRS_ARG="SPHINXDIRS=$1"
fi

# ---------- 1. make / python3 前置检查（快速失败，先于装依赖） ----------
get_major_version() {   # $1 = make 可执行文件路径，输出主版本号
    "$1" --version 2>/dev/null | head -1 | sed 's/.* \([0-9][0-9]*\)\..*/\1/'
}

select_gnumake() {
    # 内核要求 GNU Make >= 4.0；Apple 自带 /usr/bin/make 是 3.81 会被内核 Makefile 拒绝。
    # 依次尝试 Homebrew gmake（含 arm64/intel 两种安装位置）和 PATH 里的 make（若恰好 >= 4）
    local cand ver
    for cand in gmake /opt/homebrew/bin/gmake /usr/local/bin/gmake make; do
        command -v "$cand" >/dev/null 2>&1 || continue
        ver=$(get_major_version "$cand")
        [ -n "$ver" ] || continue
        if [ "$ver" -ge 4 ] 2>/dev/null; then
            echo "$cand"
            return 0
        fi
    done
    return 1
}

MAKE_BIN=$(select_gnumake)
if [ -z "$MAKE_BIN" ]; then
    echo "错误：未找到 GNU Make >= 4.0。"
    echo "  Apple 自带的 /usr/bin/make 是 3.81，内核 Makefile 会直接拒绝（GNU Make >= 4.0 is required）。"
    echo "  请先安装 Homebrew 版 GNU Make，然后重新运行本脚本："
    echo "      brew install make"
    exit 1
fi
echo "使用 make: $MAKE_BIN ($("$MAKE_BIN" --version | head -1))"

pick_python3() {
    # 优先选 >= 3.10 的新解释器（CLT 自带的 3.9 偏旧，装新 Sphinx 容易踩坑），
    # 全都找不到才退回 python3 并给出提示
    local cand ver major minor
    for cand in python3.13 python3.12 python3.11 python3.10 python3; do
        command -v "$cand" >/dev/null 2>&1 || continue
        ver=$("$cand" --version 2>/dev/null | sed 's/Python \([0-9][0-9]*\)\.\([0-9][0-9]*\).*/\1 \2/')
        major=${ver%% *}; minor=${ver#* }
        if [ -n "$major" ] && [ "$major" -gt 3 -o "$major" -eq 3 -a "$minor" -ge 10 ] 2>/dev/null; then
            echo "$cand"
            return 0
        fi
    done
    echo "python3"
}

PY_BIN=$(pick_python3)
PY_VER="$("$PY_BIN" --version 2>/dev/null)"
echo "使用 Python: $PY_BIN ($PY_VER)"
case "$PY_VER" in
    Python\ 3.10*|Python\ 3.11*|Python\ 3.12*|Python\ 3.13*) ;;
    Python\ 3.9*)
        echo "提示：检测到 CLT 自带的 Python 3.9，pip 将解析出较旧的 Sphinx 版本（可正常使用）。"
        echo "      想用更新的 Sphinx 可先执行：brew install python@3.12"
        ;;
    *)
        echo "错误：未检测到可用的 python3，请安装 Xcode CLT（xcode-select --install）或 brew install python@3.12"
        exit 1
        ;;
esac

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
    "$PY_BIN" -m venv "$VENV_DIR" && [ -x "$VENV_DIR/bin/pip" ]
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
        echo "错误：创建虚拟环境失败（macOS 需可用的 python3，可执行：xcode-select --install 或 brew install python@3.12）"
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
"$MAKE_BIN" O="$OUT_DIR" htmldocs $SPHINXDIRS_ARG $SPHINXBUILD_ARG -j"$JOBS"
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
    echo "一键用默认浏览器打开（macOS）："
    echo "  open $ENTRY"
    echo "清理文档编译产物：$MAKE_BIN O=$OUT_DIR cleandocs"
    echo "============================================================"
elif [ $BUILD_RET -eq 0 ]; then
    echo "make 返回成功，但未找到入口页 $ENTRY，请检查输出目录"
    exit 1
else
    echo "编译失败（返回码 $BUILD_RET），请检查上方错误输出"
    exit $BUILD_RET
fi
