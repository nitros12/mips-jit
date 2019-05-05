#ifndef __INSTR_H_
#define __INSTR_H_

#include <stdint.h>

#include "str_slice.h"

// representations for instructions

enum instr_type {
  INSTR_NOP,
  INSTR_ADD,
  INSTR_ADDI,
  INSTR_ANDI,
  INSTR_SRL,
  INSTR_SLL,
  INSTR_BEQ,
  INSTE_BNE
};

// d = s <op> t
struct instr_reg {
  uint8_t d, s, t;
};

// t = s <op> imm
// also used for jumps
struct instr_imm {
  uint8_t t, s;
  uint16_t imm;
};

struct instr {
  enum instr_type type;
  struct string_slice label;
  union {
    instr_reg reg_instr;
    instr_imm imm_instr;
  };
};

#endif // __INSTR_H_
