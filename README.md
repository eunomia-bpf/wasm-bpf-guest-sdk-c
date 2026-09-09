# C SDK of wasm-bpf
SDK for wasm-bpf guest programs, suitable for C programs

It contains a header file `libbpf-wasm.h`, which is mainly a replacement for the `libbpf.h` provided by `libbpf`, but with lower API replaced with `wasm-bpf`'s.

## File-descriptor attach

`bpf_set_prog_attach_target_fd(program, fd)` selects the fd-based attach path
for a skeleton program. The descriptor is a guest fd for a directory preopened
by the runtime; fd `0` is valid, and a negative fd requests the runtime's
section-based auto-attach behavior. Calling `bpf_set_prog_attach_target()`
afterward switches back to the legacy path-based attach, including an empty
path for legacy auto-attach. In either mode, a negative runtime result is
returned unchanged by `bpf_object__attach_skeleton()`.

The skeleton attach helper contains both host calls, so a rebuilt guest that
retains `bpf_object__attach_skeleton()` imports
`wasm_bpf.wasm_attach_bpf_program_fd` even when it selects the legacy path at
runtime. Such guests therefore require a wasm-bpf runtime containing the fd
attach host function from wasm-bpf #160/#163.
