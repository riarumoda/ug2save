#ifndef CARS_H
#define CARS_H

#include <stddef.h>

/* A run of bytes written at a 0-based file offset. hex_data is "AA BB CC ..." */
typedef struct {
    long offset;
    const char* hex_data;
} CarBlock;

typedef struct {
    int id;                    /* stable id, 1..CARS_COUNT (see --list-cars) */
    const char* key;           /* short name accepted by --car               */
    const char* car_name;      /* full display name                          */
    size_t block_count;
    const CarBlock* blocks;
} Car;

/* The car that is already stored in template.h (Honda Civic). */
#define TEMPLATE_CAR_ID 8

extern const Car CARS[];
extern const size_t CARS_COUNT;

/* Find a car by id ("12"), short key ("golf") or full/partial name ("Golf GTI").
 * Returns 0 and sets *out, -1 if nothing matches, -2 if the name is ambiguous. */
int cars_lookup(const char* spec, const Car** out);

#endif /* CARS_H */
