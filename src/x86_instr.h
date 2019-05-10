#ifndef __X86_INSTR_H_
#define __X86_INSTR_H_

#include <stdint.h>

/**
 * X86 instructions
 *
 * The final result after abstract instructions
 */

struct x86_instr {
    // TODO find the longest possible encoding we will have and use that
    uint8_t size;
    uint8_t bytes[4];
};

#endif // __X86_INSTR_H_
