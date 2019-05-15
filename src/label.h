#ifndef __LABEL_H_
#define __LABEL_H_

#include <stdint.h>

#include "str_slice.h"
#include "vec.h"

struct label {
    struct string_slice name;
    uint32_t id;
    int32_t code_position; // -1 if unallocated
};

DEFINE_VEC(struct label, labels);

#endif // __LABEL_H_
