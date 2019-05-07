#include <stdio.h>
#include <stdlib.h>

#include "src/instr.h"
#include "src/instr_parse.h"
#include "src/reg.h"
#include "src/str_slice.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <instr>\n", *argv);
        exit(1);
    }

    struct instr i = parse_instr(argv[1]);

    print_instr(i);
}
