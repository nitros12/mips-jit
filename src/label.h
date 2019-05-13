#ifndef __LABEL_H_
#define __LABEL_H_

#include <stdint.h>

#include "str_slice.h"

struct label {
    struct string_slice name;
    uint32_t id;
    int32_t code_position; // -1 if unallocated
};

#endif // __LABEL_H_
