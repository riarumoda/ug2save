#ifndef SAVE_MODIFIER_H
#define SAVE_MODIFIER_H

#include "../main.h"
#include "cars.h"
#include "parts-data.h"

/* All functions return 0 on success (or a byte count >= 0) and -1 on failure. */
int  modifier_rename_profile(SaveFile* sf, const char* new_name);
int  modifier_set_money(SaveFile* sf, uint32_t amount);
long modifier_unlock(SaveFile* sf, const UnlockSet* set);   /* one category only */
long modifier_inject_car(SaveFile* sf, const Car* car);

#endif /* SAVE_MODIFIER_H */
