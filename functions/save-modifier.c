#include "save-modifier.h"
#include "hex-utils.h"
#include <string.h>

int modifier_rename_profile(SaveFile* sf, const char* new_name) {
    size_t len = strlen(new_name);
    if (len == 0 || len > MAX_PROFILE_NAME) return -1;
    if (PROFILE_NAME_OFFSET + MAX_PROFILE_NAME > sf->size) return -1;
    memset(&sf->buffer[PROFILE_NAME_OFFSET], 0, MAX_PROFILE_NAME);
    memcpy(&sf->buffer[PROFILE_NAME_OFFSET], new_name, len);
    return 0;
}

int modifier_set_money(SaveFile* sf, uint32_t amount) {
    if (MONEY_OFFSET + 4 > sf->size) return -1;
    for (int i = 0; i < 4; i++)                      /* explicit little-endian */
        sf->buffer[MONEY_OFFSET + i] = (uint8_t)(amount >> (8 * i));
    return 0;
}

long modifier_unlock(SaveFile* sf, const UnlockSet* set) {
    long total = 0;
    for (size_t i = 0; i < set->run_count; i++) {
        const UnlockRun* r = &set->runs[i];
        if ((size_t)r->offset + r->length > sf->size) return -1;
        memset(&sf->buffer[r->offset], 0xFF, r->length);
        total += r->length;
    }
    return total;
}

long modifier_inject_car(SaveFile* sf, const Car* car) {
    long total = 0;
    if (!car) return -1;
    for (size_t i = 0; i < car->block_count; i++) {
        long n = apply_hex_string(sf, car->blocks[i].offset, car->blocks[i].hex_data);
        if (n < 0) return -1;
        total += n;
    }
    return total;
}
