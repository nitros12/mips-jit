#ifndef __STR_SLICE_H_
#define __STR_SLICE_H_

#include <stdlib.h>

#define NULL_SLICE(S) ((S).s == NULL)

struct string_slice {
  const char *s;
  size_t len;
};

#endif // __STR_SLICE_H_
