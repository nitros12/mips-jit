#ifndef __X86_REG_H_
#define __X86_REG_H_

#include <stdbool.h>

/**
 * X86 registers.
 */

enum __attribute__((__packed__)) x86_reg_type {
    EAX = 0,
    ECX, // NOTE: EAX and ECX is reserved for temporaries
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
};

/**
 * Array of allocatable x86 registers
 */
extern const enum x86_reg_type linear_free_x86_reg_map[];
extern const int num_free_x86_regs;

extern const char *const x86_reg_type_names[];

/**
 * Registers which are 'new' (meaning they need a different encoding sometimes)
 */
extern const bool x86_reg_is_new[];

#endif // __X86_REG_H_
