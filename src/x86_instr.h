#ifndef __X86_INSTR_H_
#define __X86_INSTR_H_

#include <stdint.h>

#include "vec.h"
#include "x86_reg.h"
#include "label.h"

/**
 * X86 instructions
 *
 * The final result after abstract instructions
 */

enum __attribute__((__packed__)) x86_instr_type {
    ZERO_REG,      // xor REG, REG
    MOV_REG_IMM,   // mov REG0, IMM
    MOV_STACK_IMM, // mov [epb - STACK], IMM
    MOV_REG_REG,   // mov REG0, REG1
    MOV_REG_STACK, // mov REG0, [epb - STACK]
    MOV_STACK_REG, // mov [epb - STACK], REG0
    ADD_REG_REG,   // add REG0, REG1 (reg0 <- reg0 + reg1)
    // NOTE: currently all <INSTR>_REG_STACK instructions are not used/
    // implemented but that's a future compilation opportunity
    /* ADD_REG_STACK, // add REG0, [epb - STACK] (reg0 <- reg0 + [epb - stack])
     */
    AND_REG_REG,
    /* AND_REG_STACK, */
    SHR_REG_IMM,
    /* SRL_REG_STACK, */
    SHL_REG_IMM,
    /* SLL_REG_STACK, */
    CMP_REG_REG,
    /* CMP_REG_STACK */
    JUMP
};

struct x86_reg {
    enum x86_reg_type reg;
};

struct x86_reg_imm {
    enum x86_reg_type dest;
    uint16_t imm;
};

struct x86_stack_imm {
    uint8_t dest_offset;
    uint16_t imm;
};

struct x86_reg_reg {
    enum x86_reg_type dest;
    enum x86_reg_type src;
};

struct x86_reg_stack {
    enum x86_reg_type dest;
    uint8_t src_offset;
};

struct x86_stack_reg {
    uint8_t dest_offset;
    enum x86_reg_type src;
};

struct x86_jump {
    bool is_eq;
    struct label *label;
};

struct x86_instr {
    enum x86_instr_type type;
    uint8_t size;
    union {
        struct x86_reg reg;
        struct x86_reg_imm reg_imm;
        struct x86_stack_imm stack_imm;
        struct x86_reg_reg reg_reg;
        struct x86_reg_stack reg_stack;
        struct x86_stack_reg stack_reg;
        struct x86_jump jump;
    };
};

DEFINE_VEC(struct x86_instr, x86_instr);

struct x86_instr construct_zero_reg(enum x86_reg_type reg);
struct x86_instr construct_mov_reg_imm(enum x86_reg_type dest, uint16_t imm);
struct x86_instr construct_mov_stack_imm(uint8_t dest_offset, uint16_t imm);
struct x86_instr construct_mov_reg_reg(enum x86_reg_type dest,
                                       enum x86_reg_type src);
struct x86_instr construct_mov_reg_stack(enum x86_reg_type dest,
                                         uint8_t src_offset);
struct x86_instr construct_mov_stack_reg(uint8_t dest_offset,
                                         enum x86_reg_type src);
struct x86_instr construct_add_reg_reg(enum x86_reg_type dest,
                                       enum x86_reg_type src);
struct x86_instr construct_and_reg_reg(enum x86_reg_type dest,
                                       enum x86_reg_type src);
struct x86_instr construct_shr_reg_imm(enum x86_reg_type reg,
                                       uint16_t imm);
struct x86_instr construct_shl_reg_imm(enum x86_reg_type reg,
                                       uint16_t imm);
struct x86_instr construct_cmp_reg_reg(enum x86_reg_type dest,
                                       enum x86_reg_type src);
struct x86_instr construct_jump(bool is_eq, struct label *label);

#endif // __X86_INSTR_H_
