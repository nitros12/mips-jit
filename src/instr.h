#ifndef __INSTR_H_
#define __INSTR_H_

#include <stdint.h>

#include "mips_reg.h"
#include "label.h"
#include "vec.h"

// representations for instructions

enum __attribute__((__packed__)) instr_type {
    INSTR_NOP,
    INSTR_ADD,
    INSTR_ADDI,
    INSTR_ANDI,
    INSTR_SRL,
    INSTR_SLL,
    INSTR_BEQ,
    INSTR_BNE
};

enum __attribute__((__packed__)) instr_class {
    INSTR_CLASS_REG,
    INSTR_CLASS_IMM,
    INSTR_CLASS_BRANCH,
    INSTR_CLASS_NOP
};

extern const char *const instr_type_names[];

extern const enum instr_class instr_class_map[];

// d = s <op> t
struct instr_reg {
    enum reg_type d, s, t;
};

// t = s <op> imm
struct instr_imm {
    enum reg_type t, s;
    uint16_t imm;
};

// branch to label instrs
struct instr_branch {
    enum reg_type t, s;
    struct label *label;
};

struct instr {
    enum instr_type type;
    struct label *label;
    union {
        struct instr_reg reg_instr;
        struct instr_imm imm_instr;
        struct instr_branch branch_instr;
    };
};

void print_instr(struct instr *i);

DEFINE_VEC(struct instr, instr);

#endif // __INSTR_H_
