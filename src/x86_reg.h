#ifndef __X86_REG_H_
#define __X86_REG_H_

/**
 * X86 registers.
 */

enum __attribute__((__packed__)) x86_reg_type {
    EAX,
    ECX,
    EDX,
    EBX,
    R8D,
    RSI,
    RDI,
    R9D,
    R10D,
    R11D,
    R12D,
    R13D,
    R14D,
    R15D
};

extern const char *const x86_reg_type_names[];

/**
 * Registers which are 'new' (meaning they need a different encoding sometimes)
 */
extern const bool x86_reg_is_new[];

#endif // __X86_REG_H_
