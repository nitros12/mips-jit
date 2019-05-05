#ifndef __COMMON_H_
#define __COMMON_H_

#define RUNTIME_ERROR(F, ...)                                                  \
  do {                                                                         \
    fprintf(stderr, "Runtime Error (%s:%d): ", __func__, __LINE__);            \
    fprintf(stderr, F "\n", ##__VA_ARGS__);                                    \
    exit(1);                                                                   \
  } while (0)

#endif // __COMMON_H_
