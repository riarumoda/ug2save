/* Unlock sets. Generated from the original NFSU2 Profile-Creator tables.
 * Each run sets `length` bytes starting at the 0-based `offset` to 0xFF. The four sets are
 * disjoint, so they can be applied independently. */
#include "parts-data.h"

static const UnlockRun runs_body[] = {
    {0x5CD2, 20}, {0x5CEE, 8}, {0x5E0A, 100}, {0x5E8E, 48}, {0x5F06, 28}, {0x5F26, 8},
    {0x5F32, 8}, {0x64C4, 20}, {0x64E0, 8}, {0x65FC, 100}, {0x6680, 48}, {0x66F8, 28},
    {0x6718, 8}, {0x6724, 8}, {0x6CB6, 20}, {0x6CD2, 8}, {0x6DEE, 100}, {0x6E72, 48},
    {0x6EEA, 28}, {0x6F0A, 8}, {0x6F16, 8}, {0x74A8, 20}, {0x74C4, 8}, {0x75E0, 100},
    {0x7664, 48}, {0x76DC, 28}, {0x76FC, 8}, {0x7708, 8}, {0x7C9A, 20}, {0x7CB6, 8},
    {0x7DD2, 100}, {0x7E56, 48}, {0x7ECE, 28}, {0x7EEE, 8}, {0x7EFA, 8},
};

static const UnlockRun runs_performance[] = {
    {0x5DF6, 8}, {0x65E8, 8}, {0x6DDA, 8}, {0x75CC, 8}, {0x7DBE, 8},
};

static const UnlockRun runs_graphics[] = {
    {0x5D3E, 112}, {0x6530, 112}, {0x6D22, 112}, {0x7514, 112}, {0x7D06, 112},
};

static const UnlockRun runs_specialties[] = {
    {0x5CF6, 16}, {0x5D0A, 4}, {0x5D12, 12}, {0x5D26, 8}, {0x5E6E, 32}, {0x604A, 8},
    {0x64E8, 16}, {0x64FC, 4}, {0x6504, 12}, {0x6518, 8}, {0x6660, 32}, {0x683C, 8},
    {0x6CDA, 16}, {0x6CEE, 4}, {0x6CF6, 12}, {0x6D0A, 8}, {0x6E52, 32}, {0x702E, 8},
    {0x74CC, 16}, {0x74E0, 4}, {0x74E8, 12}, {0x74FC, 8}, {0x7644, 32}, {0x7820, 8},
    {0x7CBE, 16}, {0x7CD2, 4}, {0x7CDA, 12}, {0x7CEE, 8}, {0x7E36, 32}, {0x8012, 8},
};

const UnlockSet UNLOCK_SETS[] = {
    { UNLOCK_BODY, "body", "Body parts", runs_body, sizeof(runs_body) / sizeof(runs_body[0]), 1100 },
    { UNLOCK_PERFORMANCE, "performance", "Performance parts", runs_performance, sizeof(runs_performance) / sizeof(runs_performance[0]), 40 },
    { UNLOCK_GRAPHICS, "graphics", "Graphics", runs_graphics, sizeof(runs_graphics) / sizeof(runs_graphics[0]), 560 },
    { UNLOCK_SPECIALTIES, "specialties", "Specialties", runs_specialties, sizeof(runs_specialties) / sizeof(runs_specialties[0]), 400 },
};

const size_t UNLOCK_SETS_COUNT = sizeof(UNLOCK_SETS) / sizeof(UNLOCK_SETS[0]);
