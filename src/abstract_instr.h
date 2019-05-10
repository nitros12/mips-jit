#ifndef __ABSTRACT_INSTR_H_
#define __ABSTRACT_INSTR_H_

#include <stdbool.h>

#include "instr.h"
#include "mips_reg.h"
#include "str_slice.h"
#include "vec.h"

// TODO: write register allocator

/**
 * Abstract instructions
 * nop = skipped
 * add $d $s $t = $d <- $s + $t    -- binop
 * addi $t $s imm = $t <- $s + imm -- binop
 * andi $t $s imm = $t <- $s & imm -- binop
 * srl $d $t h = d <- t >> h       -- binop
 * srl $d $t h = d <- t >> h       -- binop
 * beq $s $t o = branch eq $s $t o -- branch
 * bne $s $t o = branch ne $s $t o -- branch
 */

enum __attribute__((__packed__)) abstract_storage_type {
    ABSTRACT_STORAGE_REG,
    ABSTRACT_STORAGE_MAPPED_REG,
    ABSTRACT_STORAGE_IMM
};

extern const char *const abstract_storage_type_names[];

/**
 * A mips register mapped to either an x86 register or a stack offset.
 */
struct abstract_mapped_reg {
    bool mapped_to_reg;
    union {
        uint8_t x86_reg_idx;
        uint8_t stack_offset;
    };
};

struct abstract_storage {
    enum abstract_storage_type type;
    union {
        uint16_t imm;
        enum reg_type reg;
        struct abstract_mapped_reg mapped_reg;
    };
};

enum __attribute__((__packed__)) abstract_instr_type {
    ABSTRACT_INSTR_BINOP,
    ABSTRACT_INSTR_BRANCH,
    ABSTRACT_INSTR_MOV
};

extern const char *const abstract_instr_type_names[];

enum __attribute__((__packed__)) abstract_instr_binop_op {
    ABSTRACT_INSTR_BINOP_ADD,
    ABSTRACT_INSTR_BINOP_AND,
    ABSTRACT_INSTR_BINOP_SRL,
    ABSTRACT_INSTR_BINOP_SLL
};

extern const char *const abstract_instr_binop_op_names[];

struct abstract_instr_binop {
    enum reg_type dest;
    enum abstract_instr_binop_op op;
    struct abstract_storage lhs, rhs;
};

enum __attribute__((__packed__)) abstract_instr_branch_test_type {
    ABSTRACT_INSTR_BRANCH_TEST_NE,
    ABSTRACT_INSTR_BRANCH_TEST_EQ
};

extern const char *const abstract_instr_branch_test_type_names[];

struct abstract_instr_branch {
    enum abstract_instr_branch_test_type type;
    struct abstract_storage lhs, rhs;
    struct string_slice label; // we still use branch labels at this point
};

struct abstract_instr_mov {
    enum reg_type dest;
    struct abstract_storage source;
};

struct abstract_instr {
    enum abstract_instr_type type;
    union {
        struct abstract_instr_binop binop;
        struct abstract_instr_branch branch;
        struct abstract_instr_mov mov;
    };
};

/**
 * Translate MIPS instructions into our abstract instructions.
 */
struct abstract_instr_vec *translate_instructions(struct instr_vec *instrs);

/**
 * Run the optimisation pass over abstract instructions.
 */
void optimise_abstract_instrs(struct abstract_instr_vec *instrs);

void print_abstract_instr(struct abstract_instr i);

DEFINE_VEC(struct abstract_instr, abstract_instr);

#endif // __ABSTRACT_INSTR_H_
