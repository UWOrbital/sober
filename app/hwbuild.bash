#!/usr/bin/env bash

set -euo pipefail

script_dir=$( cd -- "$( dirname -- "$0" )" &> /dev/null && pwd )
project_dir=$( cd -- "${script_dir}/.." &> /dev/null && pwd )
project_name="SOBER-MCU"
config="Debug"
clean="false"
workspace_dir="${TMPDIR:-/tmp}/stm32cubeide-workspace"

usage() {
    echo "Usage: $0 [-c] [-f Debug|Release]"
    exit 1
}

validate_config() {
    case "$1" in
        Debug|Release)
            ;;
        *)
            echo "error: unsupported configuration '$1'" >&2
            usage
            ;;
    esac
}

while getopts ":cf:h" opt; do
    case "${opt}" in
        c)
            clean="true"
            ;;
        f)
            config="${OPTARG}"
            ;;
        h)
            usage
            ;;
        :|?)
            usage
            ;;
    esac
done

validate_config "$config"

if ! command -v stm32cubeide >/dev/null 2>&1; then
    echo "error: stm32cubeide is not available on PATH" >&2
    exit 1
fi

if [[ ! -f "${project_dir}/.project" || ! -f "${project_dir}/.cproject" ]]; then
    echo "error: expected STM32CubeIDE project files were not found at ${project_dir}" >&2
    exit 1
fi

if [[ "$clean" == "true" ]]; then
    rm -rf "$workspace_dir"
fi

mkdir -p "$workspace_dir"

echo "Running STM32CubeIDE headless build for ${project_name}/${config}..."
stm32cubeide --launcher.suppressErrors -nosplash \
    -application org.eclipse.cdt.managedbuilder.core.headlessbuild \
    -data "$workspace_dir" \
    -import "$project_dir" \
    -cleanBuild "${project_name}/${config}"
