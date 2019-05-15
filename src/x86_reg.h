#ifndef __X86_REG_H_
#define __X86_REG_H_

#include <stdbool.h>

/**
 * X86 registers.
 */

enum __attribute__((__packed__)) x86_reg_type {
    EAX = 0,
    ECX,
    EDX,
    EBX,
    ESI = 6,
    EDI,
    R8D,
    R9D,
    R10D,
    R11D,
    R12D,
    R13D,
    R14D,
    R15D,

    SMALLEST_X86_REG = ECX, // EAX is reserved for temporaries
    LARGEST_X86_REG = R15D
};

extern const char *const x86_reg_type_names[];

/**
 * Registers which are 'new' (meaning they need a different encoding sometimes)
 */
extern const bool x86_reg_is_new[];

#endif // __X86_REG_H_
