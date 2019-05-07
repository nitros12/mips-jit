#ifndef __REG_H_
#define __REG_H_

#include <stdint.h>

// http://www.cs.uwm.edu/classes/cs315/Bacon/Lecture/HTML/ch05s03.html

enum __attribute__((__packed__)) reg_type {
    REG_ZERO,
    REG_AT, // pseudo instruction reserved (error if we see this)
    REG_V0,
    REG_V1,
    REG_A0,
    REG_A1,
    REG_A2,
    REG_A3,
    REG_T0,
    REG_T1,
    REG_T2,
    REG_T3,
    REG_T4,
    REG_T5,
    REG_T6,
    REG_T7,
    REG_S0,
    REG_S1,
    REG_S2,
    REG_S3,
    REG_S4,
    REG_S5,
    REG_S6,
    REG_S7,
    REG_T8,
    REG_T9,
    REG_K0, // reserved (error if we see this)
    REG_K1, // reserved (error if we see this)
    REG_GP, // global area pointer (error if we see this)
    REG_SP, // stack pointer (error if we see this)
    REG_FP, // frame pointer (error if we see this)
    REG_RA  // return address (error if we see this)
};

extern const char *const reg_type_names[];

#endif // __REG_H_
