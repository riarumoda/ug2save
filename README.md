# UG2Save

Command-line save profile creator for Need for Speed: Underground 2.
Plain C, builds on Linux, macOS and Windows (MinGW).


## Build

The only dependencies are `gcc` or `clang` + `make`. Then continue with:

    make            # builds out/ug2save
    make CC=clang   # builds out/ug2save with clang
    make check      # runs the tests
    make clean      # removes object files (keeps out/ug2save)
    make mrproper   # removes everything generated (out/)

## Usage

    ug2save -n NAME [-o DIR] [-m MONEY] [-c CAR] [-b] [-p] [-g] [-s] [-f] [-q]
    ug2save -l          # list cars       
    ug2save -u          # list unlock sets
    ug2save --help      # shows help

The profile is written to `DIR/NAME/NAME` (point `-o` at your "NFS Underground 2"
save folder). Without `-c` the car stored in `template.h` (Honda Civic) is kept.
Cars can be given by id, short name or part of the name: `-c 23`, `-c 350`, `-c "golf"`.

Unlocks are independent and off by default: `-b` body parts, `-p` performance,
`-g` graphics, `-s` specialties.

A warning will shows up if you used performance and graphics unlockables,
as it can causes issues after longterm gameplay.

Exit codes: 0 ok, 1 runtime error, 2 usage error.

## Layout

    main.c, main.h               entry point, constants (offsets, limits)
    template.h                   the base save
    functions/cli.[ch]           option parsing
    functions/cars.[ch]          31 cars: id, short name, name, data blocks
    functions/parts-data.[ch]    the four unlock sets (runs of 0xFF bytes)
    functions/hex-utils.[ch]     strict hex-string parser
    functions/save-modifier.[ch] name / money / car / unlock writers
    tests/check.sh               used by `make check`

## Template replacement

I recommend you to finish the early 3 races, bring Rachel's car into the Car Lot, and pick a civic as a car.   
Save the game, grab your savefile and rename it to `TEMPLATE`.   

Install `xxd` and then type in the terminal:

    xxd -i "path/to/TEMPLATE" >> template.h
    
And now you can use your template replacement and recompile the program.   

## Disclaimer & Credits

This repository is an unofficial port meant for modern compatibility, performance, and archival purposes.   
The data, logic, it's hex values are completely taken from NFSU II Profile-Creator 1.2 by Igor Wallossek (2006).   
No copyright infringement is intended. All credit for the original design and logical architecture belongs to the original creator.   
The original tool are still available on it's respected [Gamebanana page](https://gamebanana.com/tools/5380).   
