#ifndef CLI_H
#define CLI_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
    const char* name;      /* -n  profile name                              */
    const char* outdir;    /* -o  directory the profile folder goes into    */
    const char* car;       /* -c  car id / key / name, NULL = template car  */
    int has_money;         /* -m  given?                                    */
    uint32_t money;
    unsigned unlocks;      /* -b -p -g -s  (UNLOCK_* flags)                 */
    int force;             /* -f  overwrite an existing profile             */
    int quiet;             /* -q  only print errors                         */
} Options;

/* Parse argv. Returns -1 if the program should continue with `o` filled in,
 * otherwise the exit code to return (0 for --help/--version/--list-*, 2 for
 * a usage error). */
int cli_parse(int argc, char** argv, Options* o);

void cli_usage(FILE* stream);

#endif /* CLI_H */
