#ifndef __ABSTRACT_INSTR_H_
#define __ABSTRACT_INSTR_H_

#include <stdbool.h>

#include "instr.h"
#include "label.h"
#include "mips_reg.h"
#include "x86_reg.h"
#include "vec.h"

/**
 * Abstract instructions
 * nop = skipped
 * add $d $s $t = $d <- $s + $t    -- binop
 * addi $t $s imm = $t <- $s + imm -- binop
 * andi $t $s imm = $t <- $s & imm -- binop
 * srl $d $t h = d <- t >> h       -- shift
 * srl $d $t h = d <- t << h       -- shift
 * beq $s $t o = branch eq $s $t o -- branch
 * bne $s $t o = branch ne $s $t o -- branch
 */

enum __attribute__((__packed__)) abstract_storage_type {
    ABSTRACT_STORAGE_REG,
    ABSTRACT_STORAGE_IMM
};

extern const char *const abstract_storage_type_names[];

struct abstract_storage {
    enum abstract_storage_type type;
    union {
        uint16_t imm;
        enum reg_type reg;
    };
};

enum __attribute__((__packed__)) abstract_instr_type {
    ABSTRACT_INSTR_BINOP,
    ABSTRACT_INSTR_BRANCH,
    ABSTRACT_INSTR_MOV,
    ABSTRACT_INSTR_SHIFT
};

extern const char *const abstract_instr_type_names[];

enum __attribute__((__packed__)) abstract_instr_binop_op {
    ABSTRACT_INSTR_BINOP_ADD,
    ABSTRACT_INSTR_BINOP_AND,
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
    struct label *label; // we still use branch labels at this point
};

struct abstract_instr_mov {
    enum reg_type dest;
    struct abstract_storage source;
};

enum __attribute__((__packed__)) abstract_instr_shift_direction {
    ABSTRACT_INSTR_SHIFT_LEFT,
    ABSTRACT_INSTR_SHIFT_RIGHT
};

struct abstract_instr_shift {
    enum abstract_instr_shift_direction direction;
    enum reg_type dest;
    enum reg_type lhs;
    uint8_t rhs;
};

struct abstract_instr {
    struct label *label;
    enum abstract_instr_type type;
    union {
        struct abstract_instr_binop binop;
        struct abstract_instr_branch branch;
        struct abstract_instr_mov mov;
        struct abstract_instr_shift shift;
    };
};

enum __attribute__((__packed__)) reg_mapping_type {
    X86_REG_MAPPED, // mapped to an x86 register
    STACK_MAPPED    // mapped to a stack offset
};

struct reg_mapping {
    bool is_mapped;
    enum reg_mapping_type type;
    union {
        enum x86_reg_type x86_reg;
        uint8_t stack_offset;
    };
};

/**
 * A mapping of mips registers to x86 registers
 */
struct mips_x86_reg_mapping {
    struct reg_mapping mapping[LARGEST_MIPS_REG + 1];
    uint8_t num_stack_spots;
};

/**
 * Translate MIPS instructions into our abstract instructions.
 */
struct abstract_instr_vec *translate_instructions(struct instr_vec *instrs);

/**
 * Run the optimisation pass over abstract instructions.
 */
void optimise_abstract_instrs(struct abstract_instr_vec *instrs);

void print_abstract_instr(struct abstract_instr *i);

DEFINE_VEC(struct abstract_instr, abstract_instr);

struct mips_x86_reg_mapping map_regs(struct abstract_instr_vec *instrs);

#endif // __ABSTRACT_INSTR_H_
