#!/usr/bin/env bash

set -euo pipefail

script_dir=$( cd -- "$( dirname -- "$0" )" &> /dev/null && pwd )
project_dir=$( cd -- "${script_dir}/.." &> /dev/null && pwd )
project_name="SOBER-MCU"
config="Debug"
workspace_dir="${TMPDIR:-/tmp}/sober-lint-workspace"
build_dir="${TMPDIR:-/tmp}/sober-lint-build"
compile_commands_file="${build_dir}/compile_commands.json"

usage() {
  echo "Usage: $0 [-c]"
  exit 1
}

while getopts ":ch" opt; do
  case "${opt}" in
    c)
      rm -rf "$workspace_dir" "$build_dir"
      ;;
    h)
      usage
      ;;
    :|?)
      usage
      ;;
  esac
done

if ! command -v stm32cubeide >/dev/null 2>&1; then
  echo "error: stm32cubeide is not available on PATH" >&2
  exit 1
fi

if ! command -v bear >/dev/null 2>&1; then
  echo "error: bear is not available on PATH" >&2
  exit 1
fi

if ! command -v clang-tidy >/dev/null 2>&1; then
  echo "error: clang-tidy is not available on PATH" >&2
  exit 1
fi

if [[ ! -f "${project_dir}/.project" || ! -f "${project_dir}/.cproject" ]]; then
  echo "error: expected STM32CubeIDE project files were not found at ${project_dir}" >&2
  exit 1
fi

mkdir -p "$workspace_dir" "$build_dir"

echo "==> Capturing compile commands via STM32CubeIDE headless build..."
bear --output "$compile_commands_file" -- stm32cubeide --launcher.suppressErrors -nosplash \
  -application org.eclipse.cdt.managedbuilder.core.headlessbuild \
  -data "$workspace_dir" \
  -import "$project_dir" \
  -cleanBuild "${project_name}/${config}"

mapfile -d '' source_files < <(find "$project_dir/Core/Src" -type f \( -name '*.c' -o -name '*.cpp' \) -print0)

if [[ ${#source_files[@]} -eq 0 ]]; then
  echo "error: no source files found for clang-tidy" >&2
  exit 1
fi

echo "==> Running clang-tidy..."
clang-tidy "${source_files[@]}" -p "$build_dir" \
  --checks='-*,clang-analyzer-*,bugprone-*,readability-*' \
  --warnings-as-errors='clang-analyzer-*,bugprone-*,readability-*' \
  --system-headers=false \
  --header-filter="^${project_dir}/Core/"

echo "==> Done linting."