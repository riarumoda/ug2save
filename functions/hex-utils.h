#ifndef HEX_UTILS_H
#define HEX_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include "../main.h"

/* Parse a space separated hex string ("0A FF 00") into the save buffer at `offset`.
 * Returns the number of bytes written, or -1 if the string is malformed or the
 * data would fall outside the buffer (nothing is written in that case). */
long apply_hex_string(SaveFile* sf, long offset, const char* hex_str);

#endif /* HEX_UTILS_H */
