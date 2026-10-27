#ifndef PARTS_DATA_H
#define PARTS_DATA_H

#include <stddef.h>
#include <stdint.h>

/* Set `length` bytes starting at 0-based `offset` to 0xFF. */
typedef struct {
    uint32_t offset;
    uint16_t length;
} UnlockRun;

/* Independent unlock categories (bit flags). The sets never overlap. */
enum {
    UNLOCK_BODY        = 1,
    UNLOCK_PERFORMANCE = 2,
    UNLOCK_GRAPHICS    = 4,
    UNLOCK_SPECIALTIES = 8
};

typedef struct {
    unsigned flag;
    const char* key;           /* name used in --list-unlocks  */
    const char* name;          /* display name                 */
    const UnlockRun* runs;
    size_t run_count;
    size_t byte_count;         /* total bytes the set touches  */
} UnlockSet;

extern const UnlockSet UNLOCK_SETS[];
extern const size_t UNLOCK_SETS_COUNT;

#endif /* PARTS_DATA_H */
