#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "abstract_instr.h"
#include "instr.h"
#include "instr_parse.h"
#include "label_storage.h"
#include "mips_reg.h"
#include "str_slice.h"
#include "vec.h"
#include "x86_instr.h"

static struct instr_vec *parse_instructions(char *source) {
    struct instr_vec *vec = instr_vec_new();

    for (char *line = strtok(source, "\n"); line != NULL;
         line = strtok(NULL, "\n")) {
        instr_vec_push(vec, parse_instr(line));
    }

    return vec;
}

/**
 * Execute a thunk.
 */
static void exec_thunk(struct thunk th, uint32_t *mapped_regs_store,
                       uint32_t *unmapped_regs) {
    // allocate a writeable region
    void *buf = mmap(NULL, th.len, PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (buf == MAP_FAILED) {
        perror("Mapping buffer failed");
    }

    memcpy(buf, th.buf, th.len);

    // then change it to rx
    if (mprotect(buf, th.len, PROT_READ | PROT_EXEC) == -1) {
        perror("Failed remapping w buffer to rx");
    }

    // and call it
    ((void (*)(uint32_t *, uint32_t *))buf)(unmapped_regs, mapped_regs_store);

    munmap(buf, th.len);
}

static void print_mapping(struct mips_x86_reg_mapping *map, uint32_t *regs_buf,
                          uint32_t *unmapped_regs_buf) {
    for (enum reg_type i = SMALLEST_MIPS_REG; i < LARGEST_MIPS_REG; i++) {
        if (!map->mapping[i].is_mapped) {
            continue;
        }

        if (map->mapping[i].type == X86_REG_MAPPED) {
            enum x86_reg_type reg = map->mapping[i].x86_reg;
            printf("%s = %s = %u\n", reg_type_names[i], x86_reg_type_names[reg],
                   regs_buf[linear_free_x86_reg_inverse_map[reg]]);
        } else {
            uint8_t offset = map->mapping[i].stack_offset;
            printf("%s = [STACK + %d] = %u\n", reg_type_names[i], offset,
                   unmapped_regs_buf[offset]);
        }
    }
}

static struct x86_instr_vec *
realize_abstract_instructions(struct mips_x86_reg_mapping *map,
                              struct abstract_instr_vec *ainstrs) {
    struct x86_instr_vec *x86_instrs = x86_instr_vec_new();
    uint32_t current_offset = 0;

    for (int i = 0; i < ainstrs->len; i++) {
        struct abstract_instr *current_instr = &ainstrs->data[i];
        if (current_instr->label != NULL) {
            resolve_label(current_instr->label, current_offset);
        }

        realize_abstract_instruction(current_instr, map, x86_instrs,
                                     &current_offset);
    }

    return x86_instrs;
}

static void print_instrs(struct instr_vec *instrs) {
    for (int i = 0; i < instrs->len; i++) {
        print_instr(&instrs->data[i]);
    }
}

static void print_abstract_instrs(struct abstract_instr_vec *ainstrs) {
    for (int i = 0; i < ainstrs->len; i++) {
        print_abstract_instr(&ainstrs->data[i]);
    }
}

static void print_x86_instrs(struct x86_instr_vec *x86_instrs) {
    for (int i = 0; i < x86_instrs->len; i++) {
        print_x86_instr(&x86_instrs->data[i]);
    }
}

static void print_encoded_instrs(struct thunk th) {
    for (int i = 0; i < th.len; i++) {
        printf("%02hhX", th.buf[i]);
    }
    printf("\n");
}

static uint32_t x86_instrs_size(struct x86_instr_vec *x86_instrs) {
    uint32_t written_bytes = 0;

    for (int i = 0; i < x86_instrs->len; i++) {
        written_bytes += x86_instrs->data[i].size;
    }

    return written_bytes;
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

    printf("\nparsed instructions:\n");
    print_instrs(instrs);

    struct abstract_instr_vec *ainstrs = translate_instructions(instrs);
    optimise_abstract_instrs(ainstrs);

    printf("\nabstract instructions:\n");
    print_abstract_instrs(ainstrs);

    struct mips_x86_reg_mapping map = map_regs(ainstrs);

    struct x86_instr_vec *x86_instrs =
        realize_abstract_instructions(&map, ainstrs);

    printf("\nx86 instructions:\n");
    print_x86_instrs(x86_instrs);

    uint32_t written_bytes = x86_instrs_size(x86_instrs);
    struct thunk encoded_instrs =
        emit_x86_instructions(x86_instrs, written_bytes);

    printf("\nencoded x86 instructions:\n");
    print_encoded_instrs(encoded_instrs);

    uint32_t *regs_buf =
        malloc(sizeof(uint32_t) * (map.num_stack_spots + num_free_x86_regs));
    exec_thunk(encoded_instrs, regs_buf, regs_buf + num_free_x86_regs);

    printf("\nfinal register values:\n");
    print_mapping(&map, regs_buf, regs_buf + num_free_x86_regs);

    free(regs_buf);
    instr_vec_free(instrs);
    abstract_instr_vec_free(ainstrs);
    x86_instr_vec_free(x86_instrs);
    free(encoded_instrs.buf);
    free(instr_buf);
}
