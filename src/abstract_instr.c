#include <stdbool.h>
#include <stdio.h>

#include "common.h"
#include "abstract_instr.h"
#include "mips_reg.h"
#include "vec.h"

MAKE_VEC(struct abstract_instr, abstract_instr);

const char *const abstract_storage_type_names[] = {
    [ABSTRACT_STORAGE_REG] = "ABSTRACT_STORAGE_REG",
    [ABSTRACT_STORAGE_IMM] = "ABSTRACT_STORAGE_IMM"};

const char *const abstract_instr_type_names[] = {
    [ABSTRACT_INSTR_BINOP] = "ABSTRACT_INSTR_BINOP",
    [ABSTRACT_INSTR_BRANCH] = "ABSTRACT_INSTR_BRANCH",
    [ABSTRACT_INSTR_MOV] = "ABSTRACT_INSTR_MOV"};

const char *const abstract_instr_binop_op_names[] = {
    [ABSTRACT_INSTR_BINOP_ADD] = "+",
    [ABSTRACT_INSTR_BINOP_AND] = "&",
    [ABSTRACT_INSTR_BINOP_SRL] = ">>",
    [ABSTRACT_INSTR_BINOP_SLL] = "<<"};

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

struct abstract_instr_vec *translate_instructions(struct instr_vec *instrs) {
    struct abstract_instr_vec *res_vec = abstract_instr_vec_new();

    for (size_t i = 0; i < instrs->len; i++) {
        struct instr instr = instrs->data[i];

        switch (instr.type) {
        case INSTR_NOP:
            if (!NULL_SLICE(instr.label)) {
                RUNTIME_ERROR("You have a NOP op with a label, don't do that...");
            }
            break;
        case INSTR_ADD:
            abstract_instr_vec_push(
                res_vec, (struct abstract_instr){
                             .type = ABSTRACT_INSTR_BINOP,
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
                res_vec, (struct abstract_instr){
                             .type = ABSTRACT_INSTR_BINOP,
                             .binop = {
                                 .dest = instr.imm_instr.t,
                                 .op = ABSTRACT_INSTR_BINOP_SRL,
                                 .lhs = translate_reg(instr.imm_instr.s),
                                 .rhs = {.type = ABSTRACT_STORAGE_IMM,
                                         .imm = instr.imm_instr.imm},
                             }});
            break;
        case INSTR_SLL:
            abstract_instr_vec_push(
                res_vec, (struct abstract_instr){
                             .type = ABSTRACT_INSTR_BINOP,
                             .binop = {
                                 .dest = instr.imm_instr.t,
                                 .op = ABSTRACT_INSTR_BINOP_SLL,
                                 .lhs = translate_reg(instr.imm_instr.s),
                                 .rhs = {.type = ABSTRACT_STORAGE_IMM,
                                         .imm = instr.imm_instr.imm},
                             }});
            break;
        case INSTR_BEQ:
            abstract_instr_vec_push(
                res_vec,
                (struct abstract_instr){
                    .type = ABSTRACT_INSTR_BRANCH,
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
    case ABSTRACT_STORAGE_MAPPED_REG:
        if (s.mapped_reg.mapped_to_reg) {
            printf("<x86_reg %d>", s.mapped_reg.x86_reg_idx);
        } else {
            printf("<stack_offset %d>", s.mapped_reg.stack_offset);
        }
        break;
    }
}

void print_abstract_instr(struct abstract_instr i) {
    printf("<ainstr %s", abstract_instr_type_names[i.type]);

    switch (i.type) {
    case ABSTRACT_INSTR_BINOP:
        printf(", %s <- ", reg_type_names[i.binop.dest]);
        print_abstract_storage(i.binop.lhs);
        printf(" %s ", abstract_instr_binop_op_names[i.binop.op]);
        print_abstract_storage(i.binop.rhs);
        printf(">\n");
        break;
    case ABSTRACT_INSTR_BRANCH:
        printf(", if ");
        print_abstract_storage(i.branch.lhs);
        printf(" %s ", abstract_instr_branch_test_type_names[i.branch.type]);
        print_abstract_storage(i.branch.rhs);
        printf(" goto %.*s>\n", (int)i.branch.label.len, i.branch.label.s);
        break;
    case ABSTRACT_INSTR_MOV:
        printf(", %s <- ", reg_type_names[i.mov.dest]);
        print_abstract_storage(i.mov.source);
        printf(">\n");
    }
}
