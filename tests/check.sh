#!/bin/sh
# Usage: check.sh BINARY WORKDIR
BIN=$1; W=$2
rm -rf "$W"; mkdir -p "$W"
fail=0
ok()   { echo "  ok    $1"; }
bad()  { echo "  FAIL  $1"; fail=1; }
byte() { od -An -tx1 -j "$2" -N "${3:-1}" "$1" | tr -d ' \n'; }
size() { wc -c < "$1" | tr -d ' '; }
expect() { [ "$2" = "$3" ] && ok "$1" || bad "$1 (got '$2', want '$3')"; }
rc() { "$@" >/dev/null 2>&1; echo $?; }

expect "help exits 0"            "$(rc "$BIN" --help)" 0
expect "no args is a usage error" "$(rc "$BIN")" 2
expect "name too long"           "$(rc "$BIN" -n TOOLONGNAME -o "$W")" 2
expect "bad name chars"          "$(rc "$BIN" -n 'a/b' -o "$W")" 2
expect "money out of range"      "$(rc "$BIN" -n AAA -m 1000001 -o "$W")" 2
expect "unknown car"             "$(rc "$BIN" -n AAA -c nosuchcar -o "$W")" 2
expect "unknown option"          "$(rc "$BIN" -n AAA --bogus -o "$W")" 2

# plain profile: only name changes
"$BIN" -q -n PLAIN -o "$W" ; F=$W/PLAIN/PLAIN
expect "file size"               "$(size "$F")" 54966
expect "name written"            "$(byte "$F" 53797 7)" 504c41494e0000
expect "body untouched"          "$(byte "$F" 23762)" 00

# money + attached/combined option styles, only body unlocked
"$BIN" -q -nT1 -m12345 -b -o "$W"; F=$W/T1/T1
expect "money little-endian"     "$(byte "$F" 41322 4)" 39300000
expect "body unlocked (0x5CD2)"  "$(byte "$F" 23762)" ff
expect "performance still locked (0x5DF6)" "$(byte "$F" 24054)" 00
expect "graphics still locked (0x5D3E)"    "$(byte "$F" 23870)" 00
expect "specialties still locked (0x5CF6)" "$(byte "$F" 23798)" 00

# every unlock set is independent
"$BIN" -q --name=T2 --performance --graphics --specialties -o "$W"; F=$W/T2/T2
expect "body locked when not asked" "$(byte "$F" 23762)" 00
expect "performance unlocked"    "$(byte "$F" 24054)" ff
expect "graphics unlocked"       "$(byte "$F" 23870)" ff
expect "specialties unlocked"    "$(byte "$F" 23798)" ff

# car injection (Opel Corsa, id 2) and lookup styles
"$BIN" -q -n CARS -c corsa -o "$W"; F=$W/CARS/CARS
expect "corsa data at 0x5870"    "$(byte "$F" 22640 4)" bc5c3043
"$BIN" -q -n CARS2 -c 2 -o "$W"; cmp -s "$F" "$W/CARS2/CARS2" 2>/dev/null
[ "$(byte "$W/CARS2/CARS2" 22640 4)" = bc5c3043 ] && ok "car by id" || bad "car by id"
expect "ambiguous car name"      "$(rc "$BIN" -n AMB -c mitsubishi -o "$W")" 2

# overwrite protection and nested output dir
expect "refuse existing profile" "$(rc "$BIN" -n PLAIN -o "$W")" 1
expect "--force overwrites"      "$(rc "$BIN" -n PLAIN -f -o "$W")" 0
expect "creates nested -o dir"   "$(rc "$BIN" -n DEEP -o "$W/a/b/c")" 0

# every car must apply cleanly
bad_car=0
for i in $(seq 1 31); do "$BIN" -q -n C$i -c $i -o "$W" || bad_car=1; done
[ $bad_car = 0 ] && ok "all 31 cars apply" || bad "all 31 cars apply"

[ $fail = 0 ] && echo "All tests passed." || { echo "Some tests FAILED."; exit 1; }
