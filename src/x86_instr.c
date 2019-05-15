#include "common.h"
#include "label.h"
#include "vec.h"
#include "x86_instr.h"
#include "x86_reg.h"

MAKE_VEC(struct x86_instr, x86_instr);

struct x86_instr construct_zero_reg(enum x86_reg_type reg) {
    // if dest is old: [31, c0 + reg]
    // if dest is new: [45, 31, c0 + reg - r8d]
    return (struct x86_instr){
        .type = ZERO_REG, .size = 2 + x86_reg_is_new[reg], .reg = {.reg = reg}};
}

struct x86_instr construct_mov_reg_imm(enum x86_reg_type dest, uint16_t imm) {
    // if dest is old: [b8 + dest, imm.0, imm.1, imm.2, imm.3]
    // if dest is new: [41, b8 + dest - r8d, imm.0, imm.1, imm.2, imm.3]

    return (struct x86_instr){.type = MOV_REG_IMM,
                              .size = 5 + x86_reg_is_new[dest],
                              .reg_imm = {.dest = dest, .imm = imm}};
}

struct x86_instr construct_mov_stack_imm(uint8_t dest_offset, uint16_t imm) {
    // NOTE: for memory offset instrs, make sure value is converted to a signed
    // type and inverted
    // [67, c7, 45, -dest_offset, imm.0, imm.1, imm.2, imm.3]

    if (dest_offset > 0x80) {
        RUNTIME_ERROR("Invalid offset, must be less than 0x80: %d",
                      dest_offset);
    }

    return (struct x86_instr){
        .type = MOV_STACK_IMM,
        .size = 7,
        .stack_imm = {.dest_offset = dest_offset, .imm = imm}};
}

struct x86_instr construct_mov_reg_reg(enum x86_reg_type dest,
                                       enum x86_reg_type src) {
    // if src and dest are old: [89,     0b11(src : 3)(dest : 3)]
    // if src is old, dest new: [41, 89, 0b11(src : 3)(dest - r8d : 3)]
    // if src is new, dest old: [44, 89, 0b11(src - r8d : 3)(dest : 3)]
    // if src and dest are new: [45, 89, 0b11(src - r8d : 3)(dest - r8d : 3)]

    return (struct x86_instr){
        .type = MOV_REG_REG,
        .size = 2 + (x86_reg_is_new[src] | x86_reg_is_new[dest]),
        .reg_reg = {.dest = dest, .src = src}};
}

struct x86_instr construct_mov_reg_stack(enum x86_reg_type dest,
                                         uint8_t src_offset) {
    // if dest is old: [67,     8b, 0b01(dest : 3)101, -src_offset]
    // if dest is new: [67, 44, 8b, 0b01(dest - r8d : 3)101, -src_offset]

    return (struct x86_instr){
        .type = MOV_REG_STACK,
        .size = 4 + x86_reg_is_new[dest],
        .reg_stack = {.dest = dest, .src_offset = src_offset}};
}

struct x86_instr construct_mov_stack_reg(uint8_t dest_offset,
                                         enum x86_reg_type src) {
    // if src is old: [67,     89, 0b01(src : 3)101, -dest_offset]
    // if src is new: [67, 44, 89, 0b01(src - r8d : 3)101, -dest_offset]

    return (struct x86_instr){
        .type = MOV_STACK_REG,
        .size = 4 + x86_reg_is_new[src],
        .reg_stack = {.dest = src, .src_offset = dest_offset}};
}

struct x86_instr construct_add_reg_reg(enum x86_reg_type dest,
                                       enum x86_reg_type src) {
    // if src and dest are old: [01,     0b11(src : 3)(dest : 3)]
    // if src is old, dest new: [41, 01, 0b11(src : 3)(dest - r8d : 3)]
    // if src is new, dest old: [44, 01, 0b11(src - r8d : 3)(dest : 3)]
    // if src and dest are new: [45, 01, 0b11(src - r8d : 3)(dest - r8d : 3)]

    return (struct x86_instr){
        .type = ADD_REG_REG,
        .size = 2 + (x86_reg_is_new[src] | x86_reg_is_new[dest]),
        .reg_reg = {.dest = dest, .src = src}};
}

struct x86_instr construct_and_reg_reg(enum x86_reg_type dest,
                                       enum x86_reg_type src) {
    // if src and dest are old: [21,     0b11(src : 3)(dest : 3)]
    // if src is old, dest new: [41, 21, 0b11(src : 3)(dest - r8d : 3)]
    // if src is new, dest old: [44, 21, 0b11(src - r8d : 3)(dest : 3)]
    // if src and dest are new: [45, 21, 0b11(src - r8d : 3)(dest - r8d : 3)]

    return (struct x86_instr){
        .type = AND_REG_REG,
        .size = 2 + (x86_reg_is_new[src] | x86_reg_is_new[dest]),
        .reg_reg = {.dest = dest, .src = src}};
}

struct x86_instr construct_shr_reg_imm(enum x86_reg_type reg, uint16_t imm) {
    // if reg is old: [c1,     0b11101(reg : 3), imm]
    // if reg is new: [41, 01, 0b11101(reg - r8d : 3)]

    return (struct x86_instr){.type = SHR_REG_IMM,
                              .size = 3 + x86_reg_is_new[reg],
                              .reg_imm = {.dest = reg, .imm = imm}};
}

struct x86_instr construct_shl_reg_imm(enum x86_reg_type reg, uint16_t imm) {
    // if reg is old: [c1,     0b11100(reg : 3), imm]
    // if reg is new: [41, 01, 0b11100(reg - r8d : 3)]

    return (struct x86_instr){.type = SHL_REG_IMM,
                              .size = 3 + x86_reg_is_new[reg],
                              .reg_imm = {.dest = reg, .imm = imm}};
}
struct x86_instr construct_cmp_reg_reg(enum x86_reg_type dest,
                                       enum x86_reg_type src) {
    // if src and dest are old: [39,     0b11(src : 3)(dest : 3)]
    // if src is old, dest new: [41, 39, 0b11(src : 3)(dest - r8d : 3)]
    // if src is new, dest old: [44, 39, 0b11(src - r8d : 3)(dest : 3)]
    // if src and dest are new: [45, 39, 0b11(src - r8d : 3)(dest - r8d : 3)]

    return (struct x86_instr){
        .type = CMP_REG_REG,
        .size = 2 + (x86_reg_is_new[src] | x86_reg_is_new[dest]),
        .reg_reg = {.dest = dest, .src = src}};
}
struct x86_instr construct_jump(bool is_eq, struct label *label) {
    // if je:  [0f, 84, 4 bytes of: offset - 6]
    // if jne: [0f, 85, 4 bytes of: offset - 6]

    return (struct x86_instr){
        .type = JUMP, .size = 6, .jump = {.is_eq = is_eq, .label = label}};
}
