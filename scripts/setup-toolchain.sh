#!/usr/bin/env bash
set -euo pipefail

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cmake_arguments=()

while (($# > 0)); do
    case "$1" in
        --force)
            cmake_arguments+=("-DFORCE=ON")
            shift
            ;;
        --qt)
            cmake_arguments+=("-DQT_ROOT=$2")
            shift 2
            ;;
        --ninja)
            cmake_arguments+=("-DNINJA_PATH=$2")
            shift 2
            ;;
        --output)
            cmake_arguments+=("-DOUTPUT=$2")
            shift 2
            ;;
        *)
            echo "未知参数：$1" >&2
            exit 2
            ;;
    esac
done

exec cmake "${cmake_arguments[@]}" -P "$script_dir/setup-toolchain.cmake"
