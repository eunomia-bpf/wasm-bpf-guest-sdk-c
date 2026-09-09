#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
build_dir=${TMPDIR:-/tmp}/wasm-bpf-guest-sdk-c-tests
mkdir -p "$build_dir"

cc=${CC:-cc}
"$cc" -std=c11 -O2 -Wall -Wextra -Werror \
    -Wno-attributes -Wno-unused-parameter -Wno-unused-function \
    -Wno-pointer-to-int-cast \
    "$root/tests/test_attach_dispatch.c" -o "$build_dir/test_attach_dispatch"
"$build_dir/test_attach_dispatch"

if [ -n "${WASI_SYSROOT:-}" ]; then
    wasi_cc=${WASI_CC:-clang}
    "$wasi_cc" --target=wasm32-wasip1 --sysroot="$WASI_SYSROOT" -O2 \
        -nostartfiles -Wl,--no-entry -Wl,--export=attach_skeleton \
        -Wl,--allow-undefined \
        "$root/tests/test_wasm_import.c" -o "$build_dir/test_wasm_import.wasm"
    grep -a -q 'wasm_bpf' "$build_dir/test_wasm_import.wasm"
    grep -a -q 'wasm_attach_bpf_program_fd' \
        "$build_dir/test_wasm_import.wasm"
fi
