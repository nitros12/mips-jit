#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdio.h>

#if defined(DEBUG)
#define DEBUG_LOG(F, ...)                                                      \
    do {                                                                       \
        fprintf(stderr, "(%s:%d): ", __func__, __LINE__);                      \
        fprintf(stderr, F "\n", ##__VA_ARGS__);                                \
    } while (0)
#else
#define DEBUG_LOG(F, ...)                                                      \
    do {                                                                       \
    } while (0)
#endif

#define RUNTIME_ERROR(F, ...)                                                  \
    do {                                                                       \
        fprintf(stderr, "Runtime Error (%s:%d): ", __func__, __LINE__);        \
        fprintf(stderr, F "\n", ##__VA_ARGS__);                                \
        exit(1);                                                               \
    } while (0)

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(*(A)))

#endif // __COMMON_H_
