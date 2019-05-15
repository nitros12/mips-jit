#ifndef __LABEL_STORAGE_H_
#define __LABEL_STORAGE_H_

#include "label.h"
#include "str_slice.h"

/**
 * Add a label to the storage, if the label already exists: the existing label
 * is returned.
 */
struct label *add_label(struct string_slice s);

/**
 * Lookup a lable, returns NULL if the label is not found.
 */
struct label *lookup_label(struct string_slice s);

#endif // __LABEL_STORAGE_H_
