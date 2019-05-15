#include "common.h"
#include "label.h"
#include "label_storage.h"
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

#define WRITE_INSTRUCTION(RESULT_INSTRS, CURRENT_OFFSET, FN, ...)              \
    do {                                                                       \
        struct x86_instr i__write_instruction = (FN)(__VA_ARGS__);             \
        *(CURRENT_OFFSET) += i__write_instruction.size;                        \
        x86_instr_vec_push((RESULT_INSTRS), i__write_instruction);             \
    } while (0)

static enum x86_reg_type ready_value(struct abstract_storage value,
                                     struct mips_x86_reg_mapping *map,
                                     enum x86_reg_type fallback_reg,
                                     struct x86_instr_vec *result_instrs,
                                     uint32_t *current_offset) {
    if (value.type == ABSTRACT_STORAGE_IMM) {
        WRITE_INSTRUCTION(result_instrs, current_offset, construct_mov_reg_imm,
                          fallback_reg, value.imm);
        return fallback_reg;
    }

    if (map->mapping[value.reg].type == STACK_MAPPED) {
        WRITE_INSTRUCTION(result_instrs, current_offset,
                          construct_mov_reg_stack, fallback_reg,
                          map->mapping[value.reg].stack_offset);
        return fallback_reg;
    }

    return map->mapping[value.reg].x86_reg;
}

static void store_value(enum x86_reg_type src, enum reg_type dest,
                        struct mips_x86_reg_mapping *map,
                        struct x86_instr_vec *result_instrs,
                        uint32_t *current_offset) {
    if (map->mapping[dest].type == STACK_MAPPED) {
        WRITE_INSTRUCTION(result_instrs, current_offset,
                          construct_mov_stack_reg,
                          map->mapping[dest].stack_offset, src);
    } else if (map->mapping[dest].x86_reg != src) {
        WRITE_INSTRUCTION(result_instrs, current_offset, construct_mov_reg_reg,
                          map->mapping[dest].x86_reg, src);
    }
}

void realize_abstract_instruction(struct abstract_instr *i,
                                  struct mips_x86_reg_mapping *map,
                                  struct x86_instr_vec *result_instrs,
                                  uint32_t *current_offset) {

    switch (i->type) {
    case ABSTRACT_INSTR_BINOP: {
        enum x86_reg_type lhs =
            ready_value(i->binop.lhs, map, EAX, result_instrs, current_offset);
        enum x86_reg_type rhs =
            ready_value(i->binop.rhs, map, ECX, result_instrs, current_offset);

        // perform:
        // if lhs != eax: mov eax, LHS;
        // add eax, RHS;
        // mov DEST, eax

        if (lhs != EAX) {
            WRITE_INSTRUCTION(result_instrs, current_offset,
                              construct_mov_reg_reg, EAX, lhs);
        }

        switch (i->binop.op) {
        case ABSTRACT_INSTR_BINOP_ADD:
            WRITE_INSTRUCTION(result_instrs, current_offset,
                              construct_add_reg_reg, EAX, rhs);
            break;
        case ABSTRACT_INSTR_BINOP_AND:

            WRITE_INSTRUCTION(result_instrs, current_offset,
                              construct_and_reg_reg, EAX, rhs);
            break;
        }

        store_value(EAX, i->binop.dest, map, result_instrs, current_offset);
        break;
    }
    case ABSTRACT_INSTR_MOV: {
        if (i->mov.source.type != ABSTRACT_STORAGE_IMM) {
            enum x86_reg_type src = ready_value(i->mov.source, map, EAX,
                                                result_instrs, current_offset);
            store_value(src, i->mov.dest, map, result_instrs, current_offset);
            break;
        }

        // special case for moving immediates
        if (map->mapping[i->mov.dest].type == X86_REG_MAPPED) {
            if (i->mov.source.imm == 0) {
                // zeroing register
                WRITE_INSTRUCTION(result_instrs, current_offset,
                                  construct_zero_reg,
                                  map->mapping[i->mov.dest].x86_reg);
            } else {
                // moving a non zero value
                WRITE_INSTRUCTION(
                    result_instrs, current_offset, construct_mov_reg_imm,
                    map->mapping[i->mov.dest].x86_reg, i->mov.source.imm);
            }
        }
        break;
    }
    case ABSTRACT_INSTR_SHIFT: {
        enum x86_reg_type val =
            ready_value((struct abstract_storage){.type = ABSTRACT_STORAGE_REG,
                                                  .reg = i->shift.lhs},
                        map, EAX, result_instrs, current_offset);

        // special case for when source == dest (we can do the in place shift on
        // the source register without swapping through eax)
        if (map->mapping[i->shift.dest].type != X86_REG_MAPPED &&
            map->mapping[i->shift.dest].x86_reg != val && val != EAX) {
            WRITE_INSTRUCTION(result_instrs, current_offset,
                              construct_mov_reg_reg, EAX, val);
            val = EAX;
        }

        if (i->shift.direction == ABSTRACT_INSTR_SHIFT_LEFT) {
            WRITE_INSTRUCTION(result_instrs, current_offset,
                              construct_shl_reg_imm, val, i->shift.rhs);
        } else {

            WRITE_INSTRUCTION(result_instrs, current_offset,
                              construct_shr_reg_imm, val, i->shift.rhs);
        }

        store_value(val, i->shift.dest, map, result_instrs, current_offset);
        break;
    }
    case ABSTRACT_INSTR_BRANCH: {
        enum x86_reg_type lhs =
            ready_value(i->binop.lhs, map, EAX, result_instrs, current_offset);
        enum x86_reg_type rhs =
            ready_value(i->binop.rhs, map, ECX, result_instrs, current_offset);

        WRITE_INSTRUCTION(result_instrs, current_offset, construct_cmp_reg_reg,
                          lhs, rhs);
        WRITE_INSTRUCTION(result_instrs, current_offset, construct_jump,
                          i->branch.type == ABSTRACT_INSTR_BRANCH_TEST_EQ,
                          i->branch.label);
        break;
    }
    }
}

uint8_t *emit_x86_instructions(struct x86_instr_vec *instrs) { return NULL; }

static void print_maybe_resolved_label(struct label *label) {
    if (label->code_position > 0) {
        printf("%d", label->code_position);
    } else {
        printf("<unresolved_label: %d>", label->id);
    }
}

void print_x86_instr(struct x86_instr *i) {
    switch (i->type) {
    case ZERO_REG:
        printf("xor %s, %s\n", x86_reg_type_names[i->reg.reg],
               x86_reg_type_names[i->reg.reg]);
        break;
    case MOV_REG_IMM:
        printf("mov %s, %d\n", x86_reg_type_names[i->reg_imm.dest],
               i->reg_imm.imm);
        break;
    case MOV_STACK_IMM:
        printf("mov [ebp - %d], %d\n", i->stack_imm.dest_offset,
               i->stack_imm.imm);
        break;
    case MOV_REG_REG:
        printf("mov %s, %s\n", x86_reg_type_names[i->reg_reg.dest],
               x86_reg_type_names[i->reg_reg.src]);
        break;
    case MOV_REG_STACK:
        printf("mov %s, [ebp - %d]\n", x86_reg_type_names[i->reg_stack.dest],
               i->reg_stack.src_offset);
        break;
    case MOV_STACK_REG:
        printf("mov [ebp - %d], %s\n", i->stack_reg.dest_offset,
               x86_reg_type_names[i->stack_reg.src]);
    case ADD_REG_REG:
        printf("add %s, %s\n", x86_reg_type_names[i->reg_reg.dest],
               x86_reg_type_names[i->reg_reg.src]);
        break;
    case AND_REG_REG:
        printf("and %s, %s\n", x86_reg_type_names[i->reg_reg.dest],
               x86_reg_type_names[i->reg_reg.src]);
        break;
    case SHR_REG_IMM:
        printf("shr %s, %d\n", x86_reg_type_names[i->reg_imm.dest],
               i->reg_imm.imm);
        break;
    case SHL_REG_IMM:
        printf("shl %s, %d\n", x86_reg_type_names[i->reg_imm.dest],
               i->reg_imm.imm);
        break;
    case CMP_REG_REG:
        printf("cmp %s, %s\n", x86_reg_type_names[i->reg_reg.dest],
               x86_reg_type_names[i->reg_reg.src]);
        break;
    case JUMP:
        printf("%s ", i->jump.is_eq ? "je" : "jne");
        print_maybe_resolved_label(i->jump.label);
        printf("\n");
        break;
    }
}
