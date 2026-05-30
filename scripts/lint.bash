#!/usr/bin/env bash

set -euo pipefail

script_dir=$( cd -- "$( dirname -- "$0" )" &> /dev/null && pwd )
project_dir=$( cd -- "${script_dir}/.." &> /dev/null && pwd )
project_name="SOBER-MCU"
config="Debug"
workspace_dir="${TMPDIR:-/tmp}/sober-lint-workspace"
build_dir="${TMPDIR:-/tmp}/sober-lint-build"
compile_commands_file="${build_dir}/compile_commands.json"
line_filter_file="${build_dir}/clang-tidy-line-filter.json"

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

# Extract USER CODE regions for clang-tidy line filtering
user_code_json='['
first_file='true'
lint_files=()
for source_file in "${source_files[@]}"; do
  mapfile -t user_ranges < <(awk '
    /\/\* USER CODE BEGIN / { begin = NR + 1; active = 1; next }
    /\/\* USER CODE END / && active { end = NR - 1; if (end >= begin) print begin ":" end; active = 0 }
  ' "$source_file")

  if [[ ${#user_ranges[@]} -eq 0 ]]; then
    continue
  fi

  if [[ "$first_file" == "false" ]]; then
    user_code_json+=','
  fi
  first_file='false'
  lint_files+=("$source_file")

  user_code_json+="{\"name\":\"${source_file#${project_dir}/}\",\"lines\":["
  first_range='true'
  for user_range in "${user_ranges[@]}"; do
    start_line=${user_range%%:*}
    end_line=${user_range##*:}
    if [[ "$first_range" == "false" ]]; then
      user_code_json+=','
    fi
    first_range='false'
    user_code_json+="[${start_line},${end_line}]"
  done
  user_code_json+=']}'
done
user_code_json+=']'

if [[ "$user_code_json" == '[]' || ${#lint_files[@]} -eq 0 ]]; then
  echo "error: no USER CODE regions found for clang-tidy filtering" >&2
  exit 1
fi

printf '%s\n' "$user_code_json" > "$line_filter_file"

echo "==> Running clang-tidy..."
clang-tidy "${lint_files[@]}" -p "$build_dir" \
  --checks='-*,clang-analyzer-*,bugprone-*,readability-*' \
  --warnings-as-errors='clang-analyzer-*,bugprone-*,readability-*' \
  --system-headers=false \
  --remove-arg=-fcyclomatic-complexity \
  --line-filter="$(cat "$line_filter_file")" \
  --header-filter="^${project_dir}/Core/"

echo "==> Done linting."