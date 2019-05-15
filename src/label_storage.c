#include <stdint.h>
#include <string.h>

#include "label_storage.h"

static struct labels_vec *labels;

void init_labels(void) __attribute__((constructor));
void init_labels(void) { labels = labels_vec_new(); }

struct label *add_label(struct string_slice s) {
    struct label *maybe_label = lookup_label(s);

    if (maybe_label != NULL) {
        return maybe_label;
    }

    // not found, add label

    labels_vec_push(
        labels,
        (struct label){.name = s, .id = labels->len, .code_position = -1});

    return &labels->data[labels->len - 1];
}

struct label *lookup_label(struct string_slice s) {
    for (size_t i = 0; i < labels->len; i++) {
        if (s.len != labels->data[i].name.len) {
            continue;
        }

        if (!strncmp(s.s, labels->data[i].name.s, s.len)) {
            return &labels->data[i];
        }
    }

    return NULL;
}

void resolve_label(struct label *label, uint32_t code_position) {
    label->code_position = code_position;
}
