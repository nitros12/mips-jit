#include <ctype.h>
#include <string.h>

#include "common.h"
#include "instr.h"
#include "instr_parse.h"
#include "label_storage.h"
#include "mips_reg.h"

/**
 * Returns the position of the first non-whitespace character in a string.
 */
static void eat_whitespace(const char **s) {
    while (isspace(**s)) {
        (*s)++;
    }
}

/**
 * Consume the next word in a string.
 */
static struct string_slice parse_word(const char **s) {
    eat_whitespace(s);
    const char *start = *s;
    size_t len = 0;

    while (!isspace(**s) && **s) {
        len++;
        (*s)++;
    }

    return (struct string_slice){.s = start, .len = len};
}

static enum reg_type parse_reg_type(const char **instr) {
    const char *initial_instr = *instr;

#define BAD_REG() RUNTIME_ERROR("Invalid register: %s", initial_instr)

    if (*(*instr)++ != '$') {
        BAD_REG();
    }

    switch (*(*instr)++) {
    case 'z':
        if (strncmp(*instr, "ero", strlen("ero")) == 0) {
            *instr += 4;
            return REG_ZERO;
        }
        BAD_REG();
    case 'v':
        switch (*(*instr)++) {
        case '0':
            return REG_V0;
        case '1':
            return REG_V1;
        default:
            BAD_REG();
        }
    case 'a':
        switch (*(*instr)++) {
        case '0':
            return REG_A0;
        case '1':
            return REG_A1;
        case '2':
            return REG_A2;
        case '3':
            return REG_A3;
        default:
            BAD_REG();
        }
    case 't':
        switch (*(*instr)++) {
        case '0':
            return REG_T0;
        case '1':
            return REG_T1;
        case '2':
            return REG_T2;
        case '3':
            return REG_T3;
        case '4':
            return REG_T4;
        case '5':
            return REG_T5;
        case '6':
            return REG_T6;
        case '7':
            return REG_T7;
        case '8':
            return REG_T8;
        case '9':
            return REG_T9;
        default:
            BAD_REG();
        }
    case 's':
        switch (*(*instr)++) {
        case '0':
            return REG_S0;
        case '1':
            return REG_S1;
        case '2':
            return REG_S2;
        case '3':
            return REG_S3;
        case '4':
            return REG_S4;
        case '5':
            return REG_S5;
        case '6':
            return REG_S6;
        case '7':
            return REG_S7;
        default:
            BAD_REG();
        }
    default:
        BAD_REG();
    }

#undef BAD_REG
}

/**
 * Parse an reg instruction, expects `instr` to point to the first parameter to
 * the instruction.
 */
static struct instr_reg parse_instr_reg(const char *instr) {
    enum reg_type d = parse_reg_type(&instr);
    eat_whitespace(&instr);

    enum reg_type s = parse_reg_type(&instr);
    eat_whitespace(&instr);

    enum reg_type t = parse_reg_type(&instr);

    return (struct instr_reg){.d = d, .s = s, .t = t};
}

/**
 * Parse an imm instruction, expects `instr` to point to the first parameter to
 * the instruction.
 */
static struct instr_imm parse_instr_imm(const char *instr) {
    enum reg_type t = parse_reg_type(&instr);
    eat_whitespace(&instr);

    enum reg_type s = parse_reg_type(&instr);
    eat_whitespace(&instr);

    uint16_t imm = atoi(instr);

    return (struct instr_imm){.s = s, .t = t, .imm = imm};
}

/**
 * Parse a branch instruction, expects `instr` to point to the first parameter
 * to the instruction.
 */
static struct instr_branch parse_instr_branch(const char *instr) {
    enum reg_type t = parse_reg_type(&instr);
    eat_whitespace(&instr);

    enum reg_type s = parse_reg_type(&instr);
    eat_whitespace(&instr);

    struct label *label = add_label(parse_word(&instr));

    return (struct instr_branch){.s = s, .t = t, .label = label};
}

/**
 * Parse a single instruction, possible with a label
 *
 * NOTE: This function assumes the instr buffer lives atleast as long as the
 * returned instruction (In effect: parse_instr<'a>(instr: &'a str) ->
 * Instr<'a>)
 */
struct instr parse_instr(const char *instr) {
    // eat all preceeding whitespace
    const char *initial_instr = instr;
    DEBUG_LOG("instr: %s", instr);

    eat_whitespace(&instr);

    DEBUG_LOG("instr: %s", instr);
    // first search for a colon, if so parse a label
    struct label *label = NULL;

    const char *label_ptr = NULL;
    if ((label_ptr = strchr(instr, ':')) != NULL) {
        struct string_slice s = {.s = instr, .len = label_ptr - instr};
        label = add_label(s);

        // consume any whitespace between the label and the start of the
        // instruction
        instr = label_ptr + 1;
        eat_whitespace(&instr);
    }

    // to find what instruction we're at, we'll sum the characters until we get
    // to a whitespace then map the result to instructions this is fine since
    // nothing clashes

    DEBUG_LOG("instr: %s", instr);
    int instr_char_sum = 0;
    while (!isspace(*instr)) {
        instr_char_sum += *instr++;
    }

    eat_whitespace(&instr);
    DEBUG_LOG("instr: %s", instr);

    switch (instr_char_sum) {
    case 333:
        return (struct instr){.type = INSTR_NOP, .label = label};
    case 297:
        return (struct instr){.type = INSTR_ADD,
                              .label = label,
                              .reg_instr = parse_instr_reg(instr)};
    case 402:
        return (struct instr){.type = INSTR_ADDI,
                              .label = label,
                              .imm_instr = parse_instr_imm(instr)};
    case 412:
        return (struct instr){.type = INSTR_ANDI,
                              .label = label,
                              .imm_instr = parse_instr_imm(instr)};
    case 337:
        return (struct instr){.type = INSTR_SRL,
                              .label = label,
                              .imm_instr = parse_instr_imm(instr)};
    case 331:
        return (struct instr){.type = INSTR_SLL,
                              .label = label,
                              .imm_instr = parse_instr_imm(instr)};
    case 312:
        return (struct instr){.type = INSTR_BEQ,
                              .label = label,
                              .branch_instr = parse_instr_branch(instr)};
    case 309:
        return (struct instr){.type = INSTR_BNE,
                              .label = label,
                              .branch_instr = parse_instr_branch(instr)};
    default:
        RUNTIME_ERROR("Invalid instruction: %s", initial_instr);
    }
}
