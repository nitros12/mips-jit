#include <stdbool.h>
#include <stdio.h>

#include "abstract_instr.h"
#include "common.h"
#include "label_storage.h"
#include "mips_reg.h"
#include "vec.h"
#include "x86_reg.h"

MAKE_VEC(struct abstract_instr, abstract_instr);

const char *const abstract_storage_type_names[] = {
    [ABSTRACT_STORAGE_REG] = "ABSTRACT_STORAGE_REG",
    [ABSTRACT_STORAGE_IMM] = "ABSTRACT_STORAGE_IMM"};

const char *const abstract_instr_type_names[] = {
    [ABSTRACT_INSTR_BINOP] = "ABSTRACT_INSTR_BINOP",
    [ABSTRACT_INSTR_BRANCH] = "ABSTRACT_INSTR_BRANCH",
    [ABSTRACT_INSTR_MOV] = "ABSTRACT_INSTR_MOV",
    [ABSTRACT_INSTR_SHIFT] = "ABSTRACT_INSTR_SHIFT"};

const char *const abstract_instr_binop_op_names[] = {
    [ABSTRACT_INSTR_BINOP_ADD] = "+", [ABSTRACT_INSTR_BINOP_AND] = "&"};

const char *const abstract_instr_branch_test_type_names[] = {
    [ABSTRACT_INSTR_BRANCH_TEST_NE] = "!=",
    [ABSTRACT_INSTR_BRANCH_TEST_EQ] = "=="};

static struct abstract_storage translate_reg(enum reg_type r) {
    if (r == REG_ZERO) {
        return (struct abstract_storage){.type = ABSTRACT_STORAGE_IMM,
                                         .imm = 0};
    }

    return (struct abstract_storage){.type = ABSTRACT_STORAGE_REG, .reg = r};
}

static enum reg_type ensure_nonzero(enum reg_type r,
                                    const char *const instr_name) {
    if (r == REG_ZERO) {
        RUNTIME_ERROR("Zero register not allowed in %s instruction",
                      instr_name);
    }

    return r;
}

struct abstract_instr_vec *translate_instructions(struct instr_vec *instrs) {
    struct abstract_instr_vec *res_vec = abstract_instr_vec_new();

    for (size_t i = 0; i < instrs->len; i++) {
        struct instr instr = instrs->data[i];

        switch (instr.type) {
        case INSTR_NOP:
            if (instr.label != NULL) {
                RUNTIME_ERROR(
                    "You have a NOP op with a label, don't do that...");
            }
            break;
        case INSTR_ADD:
            abstract_instr_vec_push(
                res_vec, (struct abstract_instr){
                             .type = ABSTRACT_INSTR_BINOP,
                             .label = instr.label,
                             .binop = {
                                 .dest = instr.reg_instr.d,
                                 .op = ABSTRACT_INSTR_BINOP_ADD,
                                 .lhs = translate_reg(instr.reg_instr.s),
                                 .rhs = translate_reg(instr.reg_instr.t),
                             }});
            break;
        case INSTR_ADDI:
            abstract_instr_vec_push(
                res_vec, (struct abstract_instr){
                             .type = ABSTRACT_INSTR_BINOP,
                             .label = instr.label,
                             .binop = {
                                 .dest = instr.imm_instr.t,
                                 .op = ABSTRACT_INSTR_BINOP_ADD,
                                 .lhs = translate_reg(instr.imm_instr.s),
                                 .rhs = {.type = ABSTRACT_STORAGE_IMM,
                                         .imm = instr.imm_instr.imm},
                             }});
            break;
        case INSTR_ANDI:
            abstract_instr_vec_push(
                res_vec, (struct abstract_instr){
                             .type = ABSTRACT_INSTR_BINOP,
                             .label = instr.label,
                             .binop = {
                                 .dest = instr.imm_instr.t,
                                 .op = ABSTRACT_INSTR_BINOP_AND,
                                 .lhs = translate_reg(instr.imm_instr.s),
                                 .rhs = {.type = ABSTRACT_STORAGE_IMM,
                                         .imm = instr.imm_instr.imm},
                             }});
            break;
        case INSTR_SRL:
            abstract_instr_vec_push(
                res_vec,
                (struct abstract_instr){
                    .type = ABSTRACT_INSTR_SHIFT,
                    .label = instr.label,
                    .shift = {.direction = ABSTRACT_INSTR_SHIFT_RIGHT,
                              .dest = instr.imm_instr.t,
                              .lhs = ensure_nonzero(instr.imm_instr.s, "SRL"),
                              .rhs = instr.imm_instr.imm}});
            break;
        case INSTR_SLL:
            abstract_instr_vec_push(
                res_vec,
                (struct abstract_instr){
                    .type = ABSTRACT_INSTR_SHIFT,
                    .label = instr.label,
                    .shift = {.direction = ABSTRACT_INSTR_SHIFT_LEFT,
                              .dest = instr.imm_instr.t,
                              .lhs = ensure_nonzero(instr.imm_instr.s, "SLL"),
                              .rhs = instr.imm_instr.imm}});
            break;
        case INSTR_BEQ:
            abstract_instr_vec_push(
                res_vec,
                (struct abstract_instr){
                    .type = ABSTRACT_INSTR_BRANCH,
                    .label = instr.label,
                    .branch = {.type = ABSTRACT_INSTR_BRANCH_TEST_EQ,
                               .lhs = translate_reg(instr.branch_instr.t),
                               .rhs = translate_reg(instr.branch_instr.s),
                               .label = instr.branch_instr.label}});
            break;
        case INSTR_BNE:
            abstract_instr_vec_push(
                res_vec,
                (struct abstract_instr){
                    .type = ABSTRACT_INSTR_BRANCH,
                    .label = instr.label,
                    .branch = {.type = ABSTRACT_INSTR_BRANCH_TEST_NE,
                               .lhs = translate_reg(instr.branch_instr.t),
                               .rhs = translate_reg(instr.branch_instr.s),
                               .label = instr.branch_instr.label}});
            break;
        }
    }

    return res_vec;
}

static bool optimise_abstract_instrs_inner(struct abstract_instr_vec *instrs) {
    bool did_change = false;

    for (size_t i = 0; i < instrs->len; i++) {
        struct abstract_instr instr = instrs->data[i];

        switch (instr.type) {
        case ABSTRACT_INSTR_BINOP:
            // transform 'd <- 0 + a' or 'd <- a + 0' into 'd <- a'
            if (instr.binop.op == ABSTRACT_INSTR_BINOP_ADD) {
                if (instr.binop.lhs.type == ABSTRACT_STORAGE_IMM &&
                    instr.binop.lhs.imm == 0) {
                    instrs->data[i] = (struct abstract_instr){
                        .type = ABSTRACT_INSTR_MOV,
                        .mov = {.dest = instr.binop.dest,
                                .source = instr.binop.rhs}};
                    did_change = true;
                } else if (instr.binop.rhs.type == ABSTRACT_STORAGE_IMM &&
                           instr.binop.rhs.imm == 0) {
                    instrs->data[i] = (struct abstract_instr){
                        .type = ABSTRACT_INSTR_MOV,
                        .mov = {.dest = instr.binop.dest,
                                .source = instr.binop.lhs}};
                    did_change = true;
                }
            }
            break;
        default:
            break;
        }
    }

    // NOTE: the currently implemented transforms will never result in more
    // possible optimisations so always return false currently, even if we did
    // make a transform.
    return false;
    // return did_change;
}

void optimise_abstract_instrs(struct abstract_instr_vec *instrs) {
    // fixpoint the optimisation loop
    while (optimise_abstract_instrs_inner(instrs)) {
    }
}

static void print_abstract_storage(struct abstract_storage s) {
    switch (s.type) {
    case ABSTRACT_STORAGE_REG:
        printf("<reg: %s>", reg_type_names[s.reg]);
        break;
    case ABSTRACT_STORAGE_IMM:
        printf("<imm: %d>", s.imm);
        break;
        /* case ABSTRACT_STORAGE_MAPPED_REG: */
        /*     if (s.mapped_reg.mapped_to_reg) { */
        /*         printf("<x86_reg %d>", s.mapped_reg.x86_reg_idx); */
        /*     } else { */
        /*         printf("<stack_offset %d>", s.mapped_reg.stack_offset); */
        /*     } */
        /*     break; */
    }
}

void print_abstract_instr(struct abstract_instr *i) {
    printf("<ainstr %s", abstract_instr_type_names[i->type]);

    if (i->label)
        printf(", label: %.*s", (int)i->label->name.len, i->label->name.s);

    switch (i->type) {
    case ABSTRACT_INSTR_BINOP:
        printf(", %s <- ", reg_type_names[i->binop.dest]);
        print_abstract_storage(i->binop.lhs);
        printf(" %s ", abstract_instr_binop_op_names[i->binop.op]);
        print_abstract_storage(i->binop.rhs);
        printf(">\n");
        break;
    case ABSTRACT_INSTR_BRANCH:
        printf(", if ");
        print_abstract_storage(i->branch.lhs);
        printf(" %s ", abstract_instr_branch_test_type_names[i->branch.type]);
        print_abstract_storage(i->branch.rhs);
        printf(" goto <label: %.*s, id: %ud>>\n",
               (int)i->branch.label->name.len, i->branch.label->name.s,
               i->branch.label->id);
        break;
    case ABSTRACT_INSTR_MOV:
        printf(", %s <- ", reg_type_names[i->mov.dest]);
        print_abstract_storage(i->mov.source);
        printf(">\n");
        break;
    case ABSTRACT_INSTR_SHIFT:
        printf(
            ", %s <- %s %s %d\n", reg_type_names[i->shift.dest],
            reg_type_names[i->shift.lhs],
            ((i->shift.direction == ABSTRACT_INSTR_SHIFT_LEFT) ? "<<" : ">>"),
            i->shift.rhs);
        break;
    }
}

struct reg_count_tup {
    enum reg_type reg;
    uint8_t count;
};

static int compare_reg_count_tup(const void *a_, const void *b_) {
    const struct reg_count_tup *a = a_;
    const struct reg_count_tup *b = b_;

    return (a->count < b->count) - (a->count > b->count);
}

void count_instr_regs(struct abstract_instr *i,
                      struct reg_count_tup mips_regs[LARGEST_MIPS_REG + 1]) {
    switch (i->type) {
    case ABSTRACT_INSTR_BINOP:
        mips_regs[i->binop.dest].count++;
        if (i->binop.rhs.type == ABSTRACT_STORAGE_REG) {
            mips_regs[i->binop.rhs.reg].count++;
        }
        if (i->binop.lhs.type == ABSTRACT_STORAGE_REG) {
            mips_regs[i->binop.lhs.reg].count++;
        }
        break;

    case ABSTRACT_INSTR_BRANCH:
        if (i->branch.rhs.type == ABSTRACT_STORAGE_REG) {
            mips_regs[i->branch.rhs.reg].count++;
        }
        if (i->branch.lhs.type == ABSTRACT_STORAGE_REG) {
            mips_regs[i->branch.lhs.reg].count++;
        }
        break;

    case ABSTRACT_INSTR_MOV:
        mips_regs[i->mov.dest].count++;
        if (i->mov.source.type == ABSTRACT_STORAGE_REG) {
            mips_regs[i->mov.source.reg].count++;
        }
        break;
    case ABSTRACT_INSTR_SHIFT:
        mips_regs[i->shift.dest].count++;
        mips_regs[i->shift.lhs].count++;
        break;
    }
}

struct mips_x86_reg_mapping map_regs(struct abstract_instr_vec *instrs) {

    // NOTE: this array starts in the order of the `reg_type` enumeration, but
    // after collecting all the registers we sort it then use the reg field.
    struct reg_count_tup mips_regs[LARGEST_MIPS_REG + 1] = {{0}};

    // NOTE: REG_ZERO should never appear in an abstract instruction
    for (enum reg_type reg = SMALLEST_MIPS_REG; reg <= LARGEST_MIPS_REG;
         reg++) {
        mips_regs[reg].reg = reg;
    }

    for (size_t i = 0; i < instrs->len; i++) {
        count_instr_regs(&instrs->data[i], mips_regs);
    }

    qsort(mips_regs, LARGEST_MIPS_REG + 1, sizeof(struct reg_count_tup),
          compare_reg_count_tup);

    struct mips_x86_reg_mapping mapping = {{{0}}};

    size_t mips_reg_idx = 0;

    for (int x86_reg_idx = 0; x86_reg_idx < num_free_x86_regs; x86_reg_idx++) {
        DEBUG_LOG("mapping %s to register %s\n",
                  reg_type_names[mips_regs[mips_reg_idx].reg],
                  x86_reg_type_names[linear_free_x86_reg_map[x86_reg_idx]]);
        mapping.mapping[mips_regs[mips_reg_idx++].reg] = (struct reg_mapping){
            .is_mapped = true,
            .type = X86_REG_MAPPED,
            .x86_reg = linear_free_x86_reg_map[x86_reg_idx]};
    }

    uint8_t stack_offset = 0;
    for (;
         mips_reg_idx <= LARGEST_MIPS_REG && mips_regs[mips_reg_idx].count > 0;
         mips_reg_idx++, stack_offset++) {
        DEBUG_LOG("mapping %s to stack offset %d\n",
                  reg_type_names[mips_regs[mips_reg_idx].reg], stack_offset);
        mapping.mapping[mips_regs[mips_reg_idx].reg] =
            (struct reg_mapping){.is_mapped = true,
                                 .type = STACK_MAPPED,
                                 .stack_offset = stack_offset};
    }

    mapping.num_stack_spots = stack_offset;

    return mapping;
}
