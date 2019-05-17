#ifndef __VEC_H_
#define __VEC_H_

#include <stdlib.h>

#define DEFINE_VEC(TYPE, NAME)                                                 \
                                                                               \
    struct NAME##_vec {                                                        \
        TYPE *data;                                                            \
        size_t cap;                                                            \
        size_t len;                                                            \
    };                                                                         \
                                                                               \
    struct NAME##_vec *NAME##_vec_new(void);                                   \
    void NAME##_vec_push(struct NAME##_vec *vec, TYPE i);                      \
    void NAME##_vec_free(struct NAME##_vec *vec)

#define MAKE_VEC(TYPE, NAME)                                                   \
    struct NAME##_vec *NAME##_vec_new(void) {                                  \
        const size_t initial_cap = 8;                                          \
                                                                               \
        TYPE *data = calloc(initial_cap, sizeof(TYPE));                        \
        struct NAME##_vec *vec = malloc(sizeof(struct NAME##_vec));            \
                                                                               \
        vec->len = 0;                                                          \
        vec->cap = initial_cap;                                                \
        vec->data = data;                                                      \
                                                                               \
        return vec;                                                            \
    }                                                                          \
                                                                               \
    void NAME##_vec_push(struct NAME##_vec *vec, TYPE i) {                     \
        if (vec->len == vec->cap) {                                            \
            vec->cap <<= 1;                                                    \
            vec->data = realloc(vec->data, vec->cap * sizeof(TYPE));           \
        }                                                                      \
                                                                               \
        vec->data[vec->len++] = i;                                             \
    }                                                                          \
                                                                               \
    void NAME##_vec_free(struct NAME##_vec *vec) {                             \
        free(vec->data);                                                       \
        free(vec);                                                             \
    }

#endif // __VEC_H_
