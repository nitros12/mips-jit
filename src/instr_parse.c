#include <ctype.h>
#include <string.h>

#include "instr_parse.h"
#include "instr.h"
#include "common.h"


char *eat_whitespace(const char *s) {
    while (isspace(*s)) s++;
}

/**
 * Parse a single instruction, possible with a label
 *
 * NOTE: This function assumes the instr buffer lives atleast as long as the returned instruction
 * (In effect: parse_instr<'a>(instr: &'a str) -> Instr<'a>)
 */
struct instr parse_instr(const char *instr) {
    // eat all preceeding whitespace

    instr = eat_whitespace(instr);

    // first search for a colon, if so parse a label

    struct string_slice s = {NULL, 0};

    const char *label_ptr = NULL;
    if ((label_ptr = strstr(instr, ":")) != NULL) {
        s.s = instr;
        s.len = label_ptr - instr;

        // consume any whitespace between the label and the start of the instruction
        instr = eat_whitespace(label_ptr + 1);
    }


    // We'll implement this by searching a trie, if we fail to parse just abort.
}
