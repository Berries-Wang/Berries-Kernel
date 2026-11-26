#!/bin/bash
# 把 NVIDIA resources.nvidia.com 上的白皮书/数据手册下载到本地书库 007.BOOKs/098.NVIDIA。
#
# 这类链接（如 https://resources.nvidia.com/en-us-hopper-architecture/nvidia-h100-tensor-c）
# 只是一个在线阅读器页面，正文抽不出来，真正的 PDF 地址写在页面源码里。
# 流程: 取页面 -> 从源码里找出 PDF 地址 -> 下到临时目录 -> 校验 -> 移入书库（校验不过不落盘）。
# PDF 直链也可以直接给。
#
# 注意: 变量后面紧跟中文/全角字符时要写 ${var}。系统自带的 bash 3.2 在 LANG=C.UTF-8 下
# 会把多字节字符的首字节并进变量名（报 "unbound variable"）。

set -euo pipefail

UA='Mozilla/5.0'
here=$(cd "$(dirname "$0")" && pwd -P)
books=$here/../../007.BOOKs

usage() {
    cat <<'EOF'
用法: 000.nvidia-dl.sh <链接> [文件名] [-d 目录] [-f]

  <链接>     resources.nvidia.com 页面链接，或 PDF 直链
  [文件名]   可选，只写文件名不带路径；不填则按页面标题生成，如 nvidia-h100-gpu-whitepaper.pdf
  -d 目录    保存目录，默认 007.BOOKs/098.NVIDIA
  -f         目标文件已存在时覆盖
  -h         显示本帮助
EOF
}

die() {
    echo "错误: $*" >&2
    exit 1
}

# 文本 -> 小写连字符文件名: "NVIDIA H100 GPU Whitepaper" -> nvidia-h100-gpu-whitepaper
slug() {
    perl -CSD -pe '$_ = lc; s/[^\p{L}\p{N}]+/-/g; s/-+/-/g; s/^-+|-+$//g' <<<"$1"
}

url=''
name=''
dir=''
force=0
while [ $# -gt 0 ]; do
    case $1 in
    -h | --help) usage; exit 0 ;;
    -f | --force) force=1 ;;
    -d) [ $# -ge 2 ] || die "-d 后面要跟目录"; dir=$2; shift ;;
    -*) usage >&2; die "不认识的选项: $1" ;;
    *)
        if [ -z "$url" ]; then
            url=$1
        elif [ -z "$name" ]; then
            name=$1
        else
            die "多余的参数: $1"
        fi
        ;;
    esac
    shift
done
[ -n "$url" ] || { usage >&2; exit 1; }
if [ -z "$dir" ]; then
    [ -d "$books" ] || die "没找到书库目录 ${books}，请用 -d 指定保存目录"
    dir=$books/098.NVIDIA
fi

work=$(mktemp -d "${TMPDIR:-/tmp}/nvidia-dl.XXXXXX")
trap 'rm -rf "$work"' EXIT

# 第一步只取"页面"；给的是 PDF 直链的话，取回来的就是 PDF 本身
if ! curl -sSL -A "$UA" --connect-timeout 20 --max-time 120 --retry 2 -o "$work/fetched" "$url"; then
    die "打不开链接: $url"
fi

title=''
doc=$work/doc.pdf
if [ "$(head -c 5 "$work/fetched")" = '%PDF-' ]; then
    pdf_url=$url
    mv "$work/fetched" "$doc"
else
    # 从页面源码里取标题和 PDF 地址。找 PDF 地址的三种写法依次尝试，先匹配到的算:
    #   data-source-url  阅读器容器上的属性，就是当前这份文档
    #   originalUrl      同一地址的 JS 变量
    #   iframe src       浏览器不支持 JS 时的兜底
    # 标题、PDF 地址各占一行（标题在前）；找不到 PDF 地址时 perl 以非零退出
    parsed=$(perl -0777 -ne '
        my $src = /data-source-url="([^"]+)"/ ? $1 : /var originalUrl = "([^"]+)"/ ? $1 : /<iframe[^>]*\ssrc="([^"]+)"/ ? $1 : "";
        exit 1 unless length $src;
        my $title = /property="og:title"[^>]*\scontent="([^"]*)"/i ? $1 : /<title>\s*(.*?)\s*<\/title>/is ? $1 : "";
        $title =~ s/&amp;/ and /g;
        $title =~ s/&#(\d+);/chr($1)/ge;
        $title =~ s/&\w+;//g;
        $title =~ s/\s+/ /g;
        print "$title\n$src";
    ' "$work/fetched") || die "页面里没找到 PDF 地址（这个资源可能不是 PDF，或页面结构变了）: $url"
    title=${parsed%%$'\n'*}
    pdf_url=${parsed#*$'\n'}
fi

# 没给文件名: 页面链接用页面标题，直链用 URL 里的文件名
base=${url##*/}
base=${base%%\?*}
if [ -z "$name" ] && [ -n "$title" ]; then
    name=$(slug "$title")
elif [ -z "$name" ] && { [ "${base##*.}" = pdf ] || [ "${base##*.}" = PDF ]; }; then
    name=$base
elif [ -z "$name" ]; then
    name=$(slug "$base")
fi
name=${name##*/}
name=${name:-nvidia-document}
case $name in *.pdf | *.PDF) ;; *) name=$name.pdf ;; esac

mkdir -p "$dir"
dir=$(cd "$dir" && pwd -P) # 之后一律用绝对路径，方便复制进笔记
dest=$dir/$name
if [ -e "$dest" ] && [ "$force" -eq 0 ]; then
    die "文件已存在: ${dest}（换个文件名，或加 -f 覆盖）"
fi

if [ ! -f "$doc" ]; then
    if ! curl -sSL -A "$UA" --connect-timeout 20 --retry 2 -o "$doc" "$pdf_url"; then
        die "PDF 下载失败: $pdf_url"
    fi
fi

# 校验: 文件头必须是 %PDF-；文件尾附近要有 %%EOF，没有多半是没下完整
[ "$(head -c 5 "$doc")" = '%PDF-' ] || die "下载到的不是 PDF: $url"
if ! tail -c 1024 "$doc" | grep -q '%%EOF'; then
    echo "警告: 文件尾没有 %%EOF，可能没下完整，打开确认一下" >&2
fi

mv -f "$doc" "$dest"

if [ -n "$title" ]; then
    echo "页面标题: $title"
fi
size=$(wc -c < "$dest" | tr -d ' ')
echo "已保存: ${dest} ($(LC_ALL=C awk -v b="$size" 'BEGIN { printf "%.1f MB", b / 1048576 }'), ${size} 字节)"
