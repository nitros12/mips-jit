#include <stdio.h>

#include "instr.h"
#include "mips_reg.h"
#include "vec.h"

MAKE_VEC(struct instr, instr);

const char *const instr_type_names[] = {
    [INSTR_NOP] = "INSTR_NOP",   [INSTR_ADD] = "INSTR_ADD",
    [INSTR_ADDI] = "INSTR_ADDI", [INSTR_ANDI] = "INSTR_ANDI",
    [INSTR_SRL] = "INSTR_SRL",   [INSTR_SLL] = "INSTR_SLL",
    [INSTR_BEQ] = "INSTR_BEQ",   [INSTR_BNE] = "INSTR_BNE"};

const enum instr_class instr_class_map[] = {
    [INSTR_NOP] = INSTR_CLASS_NOP,    [INSTR_ADD] = INSTR_CLASS_REG,
    [INSTR_ADDI] = INSTR_CLASS_IMM,   [INSTR_ANDI] = INSTR_CLASS_IMM,
    [INSTR_SRL] = INSTR_CLASS_IMM,    [INSTR_SLL] = INSTR_CLASS_IMM,
    [INSTR_BEQ] = INSTR_CLASS_BRANCH, [INSTR_BNE] = INSTR_CLASS_BRANCH};

void print_instr(struct instr i) {
    printf("<instr %s", instr_type_names[i.type]);

    if (i.label)
        printf(", label: %.*s", (int)i.label->name.len, i.label->name.s);

    switch (instr_class_map[i.type]) {
    case INSTR_CLASS_NOP:
        printf(">\n");
        break;
    case INSTR_CLASS_REG:
        printf(", d: %s, s: %s, t: %s>\n", reg_type_names[i.reg_instr.d],
               reg_type_names[i.reg_instr.s], reg_type_names[i.reg_instr.t]);
        break;
    case INSTR_CLASS_IMM:
        printf(", t: %s, s: %s, imm: %u>\n", reg_type_names[i.imm_instr.t],
               reg_type_names[i.imm_instr.s], i.imm_instr.imm);
        break;
    case INSTR_CLASS_BRANCH:
        printf(", t: %s, s: %s, label: <label %.*s, id: %ud>>\n",
               reg_type_names[i.branch_instr.t],
               reg_type_names[i.branch_instr.s],
               (int)i.branch_instr.label->name.len,
               i.branch_instr.label->name.s, i.branch_instr.label->id);
        break;
    }
}
