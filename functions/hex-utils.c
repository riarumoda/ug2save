#include "hex-utils.h"
#include <ctype.h>

static int hexval(int c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

long apply_hex_string(SaveFile* sf, long offset, const char* hex_str) {
    if (offset < 0) return -1;

    /* pass 1: validate and count, so a bad string never half-applies */
    long count = 0;
    const char* p = hex_str;
    while (*p) {
        while (*p == ' ') p++;
        if (!*p) break;
        if (hexval((unsigned char)p[0]) < 0 || hexval((unsigned char)p[1]) < 0) return -1;
        if (p[2] && p[2] != ' ') return -1;
        count++;
        p += 2;
    }
    if ((size_t)offset + (size_t)count > sf->size) return -1;

    /* pass 2: write */
    long i = 0;
    p = hex_str;
    while (*p) {
        while (*p == ' ') p++;
        if (!*p) break;
        sf->buffer[offset + i++] = (uint8_t)((hexval((unsigned char)p[0]) << 4) | hexval((unsigned char)p[1]));
        p += 2;
    }
    return count;
}
