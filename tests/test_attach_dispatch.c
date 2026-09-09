#include <stdbool.h>

#include "../libbpf-wasm.h"

static int path_calls;
static int fd_calls;
static const char* last_path;
static int last_fd;
static int path_result;
static int fd_result;

int wasm_attach_bpf_program(bpf_object_skel obj,
                            const char* name,
                            const char* attach_target) {
    (void)obj;
    (void)name;
    path_calls++;
    last_path = attach_target;
    return path_result;
}

int wasm_attach_bpf_program_fd(bpf_object_skel obj,
                               const char* name,
                               int target_fd) {
    (void)obj;
    (void)name;
    fd_calls++;
    last_fd = target_fd;
    return fd_result;
}

static void reset_calls(void) {
    path_calls = 0;
    fd_calls = 0;
    last_path = (void*)1;
    last_fd = -999;
    path_result = 0;
    fd_result = 0;
}

static int attach(struct bpf_program* program) {
    struct bpf_program* program_ptr = program;
    struct bpf_prog_skeleton program_skeleton = {
        .name = "test_program",
        .prog = &program_ptr,
    };
    char data = 0;
    struct bpf_object_skeleton skeleton = {
        .data = &data,
        .data_sz = 1,
        .obj = 7,
        .prog_cnt = 1,
        .prog_skel_sz = sizeof(program_skeleton),
        .progs = &program_skeleton,
    };
    return bpf_object__attach_skeleton(&skeleton);
}

int main(void) {
    struct bpf_program program = {0};
    char path[] = "/legacy/path";
    char empty[] = "";

    reset_calls();
    assert(attach(&program) == 0);
    assert(path_calls == 1 && fd_calls == 0 && last_path == NULL);

    reset_calls();
    bpf_set_prog_attach_target(&program, path);
    assert(attach(&program) == 0);
    assert(path_calls == 1 && fd_calls == 0);
    assert(strcmp(last_path, path) == 0);

    reset_calls();
    bpf_set_prog_attach_target_fd(&program, 0);
    assert(attach(&program) == 0);
    assert(path_calls == 0 && fd_calls == 1 && last_fd == 0);

    reset_calls();
    bpf_set_prog_attach_target_fd(&program, 42);
    assert(attach(&program) == 0);
    assert(path_calls == 0 && fd_calls == 1 && last_fd == 42);

    reset_calls();
    bpf_set_prog_attach_target_fd(&program, -1);
    assert(attach(&program) == 0);
    assert(path_calls == 0 && fd_calls == 1 && last_fd == -1);

    reset_calls();
    bpf_set_prog_attach_target_fd(&program, 8);
    bpf_set_prog_attach_target(&program, path);
    assert(attach(&program) == 0);
    assert(path_calls == 1 && fd_calls == 0);
    assert(strcmp(last_path, path) == 0);

    reset_calls();
    bpf_set_prog_attach_target_fd(&program, 8);
    bpf_set_prog_attach_target(&program, empty);
    assert(attach(&program) == 0);
    assert(path_calls == 1 && fd_calls == 0 && last_path == NULL);

    reset_calls();
    bpf_set_prog_attach_target(&program, path);
    bpf_set_prog_attach_target_fd(&program, 9);
    assert(attach(&program) == 0);
    assert(path_calls == 0 && fd_calls == 1 && last_fd == 9);

    reset_calls();
    path_result = -17;
    bpf_set_prog_attach_target(&program, path);
    assert(attach(&program) == -17);

    reset_calls();
    fd_result = -22;
    bpf_set_prog_attach_target_fd(&program, 3);
    assert(attach(&program) == -22);

    return 0;
}
