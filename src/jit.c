#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "instr.h"
#include "instr_parse.h"
#include "reg.h"
#include "str_slice.h"

// simple vector of instructions
struct instr_vec {
    struct instr *data;
    size_t cap;
    size_t len;
};

static struct instr_vec *instr_vec_new(void) {
    const size_t initial_cap = 8;

    struct instr *data = calloc(initial_cap, sizeof(struct instr));
    struct instr_vec *vec = malloc(sizeof(struct instr_vec));

    vec->len = 0;
    vec->cap = initial_cap;
    vec->data = data;

    return vec;
}

static void instr_vec_push(struct instr_vec *vec, struct instr i) {
    if (vec->len == vec->cap) {
        vec->cap <<= 1;
        vec->data = reallocarray(vec->data, vec->cap, sizeof(struct instr));
    }

    vec->data[vec->len++] = i;
}

static void instr_vec_free(struct instr_vec *vec) {
    free(vec->data);
    free(vec);
}

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

    struct instr_vec *instrs = parse_instructions(instr_buf);

    for (int i = 0; i < instrs->len; i++) {
        print_instr(instrs->data[i]);
    }
}
