#include <stdbool.h>

#include "../libbpf-wasm.h"

int attach_skeleton(struct bpf_object_skeleton* skeleton) {
    return bpf_object__attach_skeleton(skeleton);
}
