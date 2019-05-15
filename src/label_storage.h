#ifndef __LABEL_STORAGE_H_
#define __LABEL_STORAGE_H_

#include <stdint.h>

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

/**
 * Resolve a label, declaring it's offset.
 */
void resolve_label(struct label *label, uint32_t code_position);

#endif // __LABEL_STORAGE_H_
