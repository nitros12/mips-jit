#include "x86_reg.h"

const enum x86_reg_type linear_free_x86_reg_map[] = {
    EDX, EBX, ESI, EDI, R8D, R9D, R10D, R11D, R12D, R13D, R14D, R15D,
};

const int num_free_x86_regs = 12;

const char *const x86_reg_type_names[] = {
    [EAX] = "EAX",   [ECX] = "ECX",   [EDX] = "EDX",   [EBX] = "EBX",
    [ESI] = "RSI",   [EDI] = "EDI",   [R8D] = "R8D",   [R9D] = "R9D",
    [R10D] = "R10D", [R11D] = "R11D", [R12D] = "R12D", [R13D] = "R13D",
    [R14D] = "R14D", [R15D] = "R15D"};

const bool x86_reg_is_new[] = {
    [EAX] = false, [ECX] = false, [EDX] = false, [EBX] = false, [ESI] = false,
    [EDI] = false, [R8D] = true,  [R9D] = true,  [R10D] = true, [R11D] = true,
    [R12D] = true, [R13D] = true, [R14D] = true, [R15D] = true};
