#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "abstract_instr.h"
#include "instr.h"
#include "instr_parse.h"
#include "mips_reg.h"
#include "str_slice.h"
#include "vec.h"

static struct instr_vec *parse_instructions(char *source) {
    struct instr_vec *vec = instr_vec_new();

    for (char *line = strtok(source, "\n"); line != NULL;
         line = strtok(NULL, "\n")) {
        instr_vec_push(vec, parse_instr(line));
    }

    return vec;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input file>\n", *argv);
    }

    struct stat st;
    if (stat(argv[1], &st)) {
        perror("Failed statting source file");
    }

    char *instr_buf = malloc(st.st_size);

    FILE *instr_file = fopen(argv[1], "r");
    if (!instr_file) {
        perror("Failed opening source file");
    }

    fread(instr_buf, st.st_size, 1, instr_file);
    fclose(instr_file);

    struct instr_vec *instrs = parse_instructions(instr_buf);

    for (int i = 0; i < instrs->len; i++) {
        print_instr(instrs->data[i]);
    }

    struct abstract_instr_vec *ainstrs = translate_instructions(instrs);

    instr_vec_free(instrs);

    optimise_abstract_instrs(ainstrs);

    for (int i = 0; i < ainstrs->len; i++) {
        print_abstract_instr(ainstrs->data[i]);
    }

    abstract_instr_vec_free(ainstrs);

    free(instr_buf);
}
