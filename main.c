#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0777)
#endif

#include "main.h"
#include "template.h"                    /* TEMPLATE[] / TEMPLATE_len */
#include "functions/cli.h"
#include "functions/cars.h"
#include "functions/parts-data.h"
#include "functions/save-modifier.h"

static int file_exists(const char* path) {
    FILE* f = fopen(path, "rb");
    if (f) { fclose(f); return 1; }
    return 0;
}

/* mkdir -p */
static int ensure_dir(const char* path) {
    char tmp[1024];
    size_t n = strlen(path);
    if (n == 0 || n >= sizeof tmp) return -1;
    memcpy(tmp, path, n + 1);
    for (size_t i = 1; i <= n; i++) {
        if (tmp[i] == '/' || tmp[i] == '\\' || tmp[i] == '\0') {
            char c = tmp[i];
            if (c != '\0' && (tmp[i - 1] == '/' || tmp[i - 1] == '\\' || tmp[i - 1] == ':')) continue;
            tmp[i] = '\0';
            if (MKDIR(tmp) != 0 && errno != EEXIST) return -1;
            tmp[i] = c;
        }
    }
    return 0;
}

static int valid_name(const char* name, const char** why) {
    size_t n = strlen(name);
    if (n < 1 || n > MAX_PROFILE_NAME) { *why = "must be 1-7 characters"; return 0; }
    for (size_t i = 0; i < n; i++) {
        unsigned char c = (unsigned char)name[i];
        if (c < 0x21 || c > 0x7E || strchr("/\\:*?\"<>|", c)) {
            *why = "may only contain printable ASCII without spaces or / \\ : * ? \" < > |"; return 0; }
    }
    if (name[0] == '.') { *why = "must not start with '.'"; return 0; }
    static const char* const reserved[] = { "CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3", "COM4", "LPT1", "LPT2", "LPT3" };
    for (size_t i = 0; i < sizeof reserved / sizeof *reserved; i++) {
        const char* r = reserved[i]; size_t k = 0;
        while (r[k] && name[k] && toupper((unsigned char)name[k]) == r[k]) k++;   /* case-insensitive */
        if (!r[k] && !name[k]) { *why = "is a reserved name on Windows"; return 0; }
    }
    return 1;
}

int main(int argc, char** argv) {
    Options opt;
    int rc = cli_parse(argc, argv, &opt);
    if (rc != -1) return rc;

    const char* why = NULL;
    if (!valid_name(opt.name, &why)) {
        fprintf(stderr, PROGRAM_NAME ": invalid profile name '%s': %s\n", opt.name, why);
        return 2;
    }

    const Car* car = NULL;
    if (opt.car) {
        int r = cars_lookup(opt.car, &car);
        if (r == -1) { fprintf(stderr, PROGRAM_NAME ": unknown car '%s' (see --list-cars)\n", opt.car); return 2; }
        if (r == -2) { fprintf(stderr, PROGRAM_NAME ": car '%s' is ambiguous, use its id (see --list-cars)\n", opt.car); return 2; }
    }

    /* Sanity-check the embedded template */
    if (sizeof(TEMPLATE) != SAVE_SIZE || memcmp(TEMPLATE, "20CM", 4) != 0) {
        fprintf(stderr, PROGRAM_NAME ": template.h is not a valid %d byte NFSU2 profile\n", SAVE_SIZE);
        return 1;
    }

    char dir[1100], path[1200];
    snprintf(dir, sizeof dir, "%s/%s", opt.outdir, opt.name);
    snprintf(path, sizeof path, "%s/%s", dir, opt.name);
    if (!opt.force && file_exists(path)) {
        fprintf(stderr, PROGRAM_NAME ": profile already exists: %s (use --force to overwrite)\n", path);
        return 1;
    }

    SaveFile save;
    save.size = sizeof(TEMPLATE);
    save.buffer = (uint8_t*)malloc(save.size);
    if (!save.buffer) { fprintf(stderr, PROGRAM_NAME ": out of memory\n"); return 1; }
    memcpy(save.buffer, TEMPLATE, save.size);

    if (!opt.quiet &&
        (opt.unlocks & UNLOCK_PERFORMANCE) && (opt.unlocks & UNLOCK_GRAPHICS))
        fprintf(stderr, PROGRAM_NAME ": warning: unlocking performance and graphics "
                        " may cause issues in the game; consider unlocking "
                        "them separately.\n");

    /* Apply modifications */
    if (modifier_rename_profile(&save, opt.name) != 0) { fprintf(stderr, PROGRAM_NAME ": cannot set profile name\n"); free(save.buffer); return 1; }
    if (opt.has_money && modifier_set_money(&save, opt.money) != 0) { fprintf(stderr, PROGRAM_NAME ": cannot set money\n"); free(save.buffer); return 1; }
    if (car && modifier_inject_car(&save, car) < 0) { fprintf(stderr, PROGRAM_NAME ": internal error: bad data for car '%s'\n", car->car_name); free(save.buffer); return 1; }
    for (size_t i = 0; i < UNLOCK_SETS_COUNT; i++)
        if ((opt.unlocks & UNLOCK_SETS[i].flag) && modifier_unlock(&save, &UNLOCK_SETS[i]) < 0) {
            fprintf(stderr, PROGRAM_NAME ": internal error: bad unlock data '%s'\n", UNLOCK_SETS[i].key);
            free(save.buffer); return 1; }

    /* Write to disk */
    if (ensure_dir(dir) != 0) { fprintf(stderr, PROGRAM_NAME ": cannot create directory '%s': %s\n", dir, strerror(errno)); free(save.buffer); return 1; }
    FILE* f = fopen(path, "wb");
    int ok = f && fwrite(save.buffer, 1, save.size, f) == save.size;
    if (f && fclose(f) != 0) ok = 0;
    if (!ok) { fprintf(stderr, PROGRAM_NAME ": cannot write '%s': %s\n", path, strerror(errno)); free(save.buffer); return 1; }
    free(save.buffer);

    if (!opt.quiet) {
        printf("Profile : %s\n", opt.name);
        printf("Car     : %s\n", car ? car->car_name : "template car (Honda Civic)");
        if (opt.has_money) printf("Money   : %u\n", (unsigned)opt.money); else printf("Money   : template value\n");
        printf("Unlocks :");
        int any = 0;
        for (size_t i = 0; i < UNLOCK_SETS_COUNT; i++)
            if (opt.unlocks & UNLOCK_SETS[i].flag) { printf(" %s", UNLOCK_SETS[i].key); any = 1; }
        printf("%s\n", any ? "" : " none");
        printf("Wrote   : %s (%zu bytes)\n", path, (size_t)SAVE_SIZE);
    }
    return 0;
}
