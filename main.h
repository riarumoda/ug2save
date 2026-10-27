#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <stddef.h>

#define PROGRAM_NAME        "ug2save"
#define PROGRAM_VERSION     "1.0"

#define MAX_PROFILE_NAME    7           /* the game only accepts 7 letters      */
#define MONEY_OFFSET        41322       /* 0xA16A, 4 bytes, little-endian       */
#define MONEY_MAX           1000000u    /* same limit as the original creator   */
#define PROFILE_NAME_OFFSET 0xD225      /* 7 bytes, NUL padded                  */
#define SAVE_SIZE           54966       /* size of a valid profile file         */

/* Represents the working buffer for the save file */
typedef struct {
    uint8_t* buffer;
    size_t size;
} SaveFile;

#endif /* MAIN_H */
