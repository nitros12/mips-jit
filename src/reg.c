#include "reg.h"

const char *const reg_type_names[] = {
    [REG_ZERO] = "REG_ZERO", [REG_AT] = "REG_AT", [REG_V0] = "REG_V0",
    [REG_V1] = "REG_V1",     [REG_A0] = "REG_A0", [REG_A1] = "REG_A1",
    [REG_A2] = "REG_A2",     [REG_A3] = "REG_A3", [REG_T0] = "REG_T0",
    [REG_T1] = "REG_T1",     [REG_T2] = "REG_T2", [REG_T3] = "REG_T3",
    [REG_T4] = "REG_T4",     [REG_T5] = "REG_T5", [REG_T6] = "REG_T6",
    [REG_T7] = "REG_T7",     [REG_S0] = "REG_S0", [REG_S1] = "REG_S1",
    [REG_S2] = "REG_S2",     [REG_S3] = "REG_S3", [REG_S4] = "REG_S4",
    [REG_S5] = "REG_S5",     [REG_S6] = "REG_S6", [REG_S7] = "REG_S7",
    [REG_T8] = "REG_T8",     [REG_T9] = "REG_T9", [REG_K0] = "REG_K0",
    [REG_K1] = "REG_K1",     [REG_GP] = "REG_GP", [REG_SP] = "REG_SP",
    [REG_FP] = "REG_FP",     [REG_RA] = "REG_RA"};
