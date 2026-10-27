#include "cli.h"
#include "cars.h"
#include "parts-data.h"
#include "../main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct { char sh; const char* lg; int arg; int id; } Opt;

enum { O_NAME = 1, O_OUT, O_MONEY, O_CAR, O_BODY, O_PERF, O_GFX, O_SPEC,
       O_FORCE, O_QUIET, O_LCARS, O_LUNL, O_HELP, O_VER };

static const Opt OPTS[] = {
    { 'n', "name",         1, O_NAME  },
    { 'o', "output",       1, O_OUT   },
    { 'm', "money",        1, O_MONEY },
    { 'c', "car",          1, O_CAR   },
    { 'b', "body",         0, O_BODY  },
    { 'p', "performance",  0, O_PERF  },
    { 'g', "graphics",     0, O_GFX   },
    { 's', "specialties",  0, O_SPEC  },
    { 'f', "force",        0, O_FORCE },
    { 'q', "quiet",        0, O_QUIET },
    { 'l', "list-cars",    0, O_LCARS },
    { 'u', "list-unlocks", 0, O_LUNL  },
    { 'h', "help",         0, O_HELP  },
    { 'V', "version",      0, O_VER   },
};
#define NOPTS (sizeof(OPTS) / sizeof(OPTS[0]))

void cli_usage(FILE* f) {
    fprintf(f,
"Usage: " PROGRAM_NAME " -n NAME [options]\n"
"Create a Need for Speed: Underground 2 save profile.\n"
"\n"
"Required:\n"
"  -n, --name NAME        profile name, 1-%d characters\n"
"\n"
"Options:\n"
"  -o, --output DIR       directory to create the profile in (default: .)\n"
"                         The profile is written to DIR/NAME/NAME.\n"
"  -m, --money N          starting money, 0-%u (default: keep template value)\n"
"  -c, --car CAR          starting car: id, short name or (part of) its name\n"
"                         (default: keep the template car; see -l)\n"
"\n"
"Unlocks (each one is independent, combine as you like):\n"
"  -b, --body             unlock all body parts\n"
"  -p, --performance      unlock all performance parts\n"
"  -g, --graphics         unlock all graphics\n"
"  -s, --specialties      unlock all specialties\n"
"\n"
"Other:\n"
"  -f, --force            overwrite the profile if it already exists\n"
"  -q, --quiet            print errors only\n"
"  -l, --list-cars        list available cars and exit\n"
"  -u, --list-unlocks     list unlock categories and exit\n"
"  -h, --help             show this help and exit\n"
"  -V, --version          show version and exit\n"
"\n"
"Short options can be combined (-bpg) and values may be attached (-n ABC, -nABC,\n"
"--name=ABC).\n"
"\n"
"Examples:\n"
"  " PROGRAM_NAME " -n ABC -m 50000 -c 350 -o saves\n"
"  " PROGRAM_NAME " -n ABC -c \"golf\" -bg -o \"$HOME/NFS Underground 2\"\n",
        MAX_PROFILE_NAME, MONEY_MAX);
}

static void list_cars(void) {
    for (size_t i = 0; i < CARS_COUNT; i++)
        printf("%2d  %-10s %s%s\n", CARS[i].id, CARS[i].key, CARS[i].car_name,
               CARS[i].id == TEMPLATE_CAR_ID ? "  (template default)" : "");
}

static void list_unlocks(void) {
    for (size_t i = 0; i < UNLOCK_SETS_COUNT; i++) {
        char sh = '?';
        for (size_t k = 0; k < NOPTS; k++) {
            int id = OPTS[k].id;
            unsigned fl = id == O_BODY ? UNLOCK_BODY : id == O_PERF ? UNLOCK_PERFORMANCE :
                          id == O_GFX ? UNLOCK_GRAPHICS : id == O_SPEC ? UNLOCK_SPECIALTIES : 0;
            if (fl && fl == UNLOCK_SETS[i].flag) sh = OPTS[k].sh;
        }
        printf("-%c  %-12s %-18s %4zu bytes\n", sh, UNLOCK_SETS[i].key,
               UNLOCK_SETS[i].name, UNLOCK_SETS[i].byte_count);
    }
}

static int bad(const char* fmt, const char* a) {
    fprintf(stderr, PROGRAM_NAME ": ");
    fprintf(stderr, fmt, a);
    fprintf(stderr, "\nTry '" PROGRAM_NAME " --help' for more information.\n");
    return 2;
}

/* returns -1 to keep going, else an exit code */
static int apply(Options* o, int id, const char* val, int* done) {
    char* end;
    switch (id) {
    case O_NAME: o->name = val; break;
    case O_OUT:  o->outdir = val; break;
    case O_CAR:  o->car = val; break;
    case O_MONEY: {
        errno = 0;
        unsigned long v = strtoul(val, &end, 10);
        if (!*val || *end || errno || val[0] == '-' || v > MONEY_MAX)
            return bad("invalid money '%s' (expected 0-1000000)", val);
        o->has_money = 1; o->money = (uint32_t)v; break; }
    case O_BODY: o->unlocks |= UNLOCK_BODY; break;
    case O_PERF: o->unlocks |= UNLOCK_PERFORMANCE; break;
    case O_GFX:  o->unlocks |= UNLOCK_GRAPHICS; break;
    case O_SPEC: o->unlocks |= UNLOCK_SPECIALTIES; break;
    case O_FORCE: o->force = 1; break;
    case O_QUIET: o->quiet = 1; break;
    case O_LCARS: list_cars();   *done = 1; return 0;
    case O_LUNL:  list_unlocks(); *done = 1; return 0;
    case O_HELP:  cli_usage(stdout); *done = 1; return 0;
    case O_VER:   printf(PROGRAM_NAME " " PROGRAM_VERSION "\n");
                  printf("Based on NFSU II Profile-Creator 1.2 by Igor Wallossek (2006)\n");
                  *done = 1; return 0;
    }
    return -1;
}

int cli_parse(int argc, char** argv, Options* o) {
    memset(o, 0, sizeof *o);
    o->outdir = ".";
    int done = 0, only_args = 0;

    for (int i = 1; i < argc; i++) {
        const char* a = argv[i];
        if (only_args || a[0] != '-' || !a[1]) return bad("unexpected argument '%s'", a);
        if (!strcmp(a, "--")) { only_args = 1; continue; }

        if (a[1] == '-') {                                   /* --long[=value] */
            const char* name = a + 2; const char* eq = strchr(name, '=');
            size_t len = eq ? (size_t)(eq - name) : strlen(name);
            const Opt* m = NULL;
            for (size_t k = 0; k < NOPTS; k++)
                if (strlen(OPTS[k].lg) == len && !strncmp(OPTS[k].lg, name, len)) { m = &OPTS[k]; break; }
            if (!m) return bad("unknown option '%s'", a);
            const char* val = NULL;
            if (m->arg) {
                if (eq) val = eq + 1;
                else if (i + 1 < argc) val = argv[++i];
                else return bad("option '%s' needs a value", a);
            } else if (eq) return bad("option '%s' takes no value", a);
            int rc = apply(o, m->id, val, &done);
            if (rc != -1) return rc;
            if (done) return 0;
        } else {                                             /* -abc / -n VALUE */
            for (const char* c = a + 1; *c; c++) {
                const Opt* m = NULL;
                for (size_t k = 0; k < NOPTS; k++) if (OPTS[k].sh == *c) { m = &OPTS[k]; break; }
                if (!m) { char t[3] = { '-', *c, 0 }; return bad("unknown option '%s'", t); }
                const char* val = NULL;
                if (m->arg) {
                    if (c[1]) val = c + 1;
                    else if (i + 1 < argc) val = argv[++i];
                    else { char t[3] = { '-', *c, 0 }; return bad("option '%s' needs a value", t); }
                }
                int rc = apply(o, m->id, val, &done);
                if (rc != -1) return rc;
                if (done) return 0;
                if (m->arg) break;
            }
        }
    }
    if (!o->name) return bad("%s", "missing required option -n NAME");
    return -1;
}
