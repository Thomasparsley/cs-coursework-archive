#!/bin/sh
# Black-box tests for pgmtoascii.
# Usage: test_pgmtoascii.sh path/to/pgmtoascii
BIN=$1
DIR=$(cd "$(dirname "$0")" && pwd)
FIX=$DIR/fixtures
OUT=$(mktemp -d "${TMPDIR:-/tmp}/pgmtest.XXXXXX") || exit 2
trap 'rm -rf "$OUT"' EXIT

pass=0
fail=0

ok()  { pass=$((pass + 1)); printf 'ok   %s\n' "$1"; }
bad() { fail=$((fail + 1)); printf 'FAIL %s: %s\n' "$1" "$2"; }

# expect_output NAME INPUT EXPECTED_FILE CHARSET
expect_output() {
    rm -f "$OUT/o.txt"
    "$BIN" "$FIX/$2" "$OUT/o.txt" "$4" > /dev/null 2>&1
    rc=$?
    if [ $rc -ne 0 ]; then bad "$1" "exit code $rc"; return; fi
    if cmp -s "$OUT/o.txt" "$3"; then ok "$1"; else bad "$1" "output differs"; fi
}

# expect_error NAME INPUT CHARSET
expect_error() {
    rm -f "$OUT/o.txt"
    "$BIN" "$FIX/$2" "$OUT/o.txt" "$3" > "$OUT/stdout.txt" 2>&1
    rc=$?
    if [ $rc -ne 1 ]; then bad "$1" "exit code $rc, expected 1"; return; fi
    if [ -s "$OUT/stdout.txt" ] && [ ! -e "$OUT/o.txt" ]; then ok "$1"
    else bad "$1" "expected a message and no output file"; fi
}

expect_output spaced_digits    spaced.pgm       "$FIX/spaced.expected"      " .oX"
expect_output unspaced_digits  unspaced.pgm     "$FIX/unspaced.expected"    " .oX"
expect_output one_line_data    oneline.pgm      "$FIX/oneline.expected"     " .oX"
expect_output comments         comments.pgm     "$FIX/comments.expected"    " .oX"
expect_output crlf             crlf.pgm         "$FIX/crlf.expected"        " .oX"
expect_output one_by_one       one_by_one.pgm   "$FIX/one_by_one.expected"  ".#"
expect_output multi_digit      multidigit.pgm   "$FIX/multidigit.expected"  "0123456789abcdef"
expect_output multi_digit_no_trailing_newline \
                               multidigit_nonl.pgm "$FIX/multidigit_nonl.expected" "0123456789a"

# Sample image: the oracle is computed independently with awk.
CS=" .-+=o*O#@"
awk -v cs="$CS" 'NR > 3 { out = ""; for (i = 1; i <= length($0); i++) out = out substr(cs, substr($0, i, 1) + 1, 1); print out }' \
    "$FIX/sample.pgm" > "$OUT/sample.expected"
expect_output sample_image sample.pgm "$OUT/sample.expected" "$CS"

# Overwriting an existing longer output file must not leave stale bytes.
printf 'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n' > "$OUT/o.txt"
"$BIN" "$FIX/one_by_one.pgm" "$OUT/o.txt" ".#" > /dev/null 2>&1
if cmp -s "$OUT/o.txt" "$FIX/one_by_one.expected"; then ok overwrite_existing_output
else bad overwrite_existing_output "stale bytes left"; fi

expect_error too_few_pixels      err_too_few.pgm            " .oX"
expect_error too_many_pixels     err_too_many.pgm           " .oX"
expect_error negative_value      err_negative.pgm           " .oX"
expect_error value_over_maxval   err_over_maxval.pgm        " .oX"
expect_error value_over_maxval_multi_digit err_over_maxval_multi.pgm "0123456789abcdef"
expect_error invalid_pixel_char  err_bad_char.pgm           " .oX"
expect_error wrong_marker        err_marker.pgm             " .oX"
expect_error incomplete_header   err_header_incomplete.pgm  " .oX"
expect_error zero_width          err_zero_width.pgm         " .oX"
expect_error empty_file          err_empty.pgm              " .oX"
expect_error charset_too_short   spaced.pgm                 " .o"
expect_error charset_too_long    spaced.pgm                 " .oXY"
expect_error empty_charset       spaced.pgm                 ""

# Missing input file
rm -f "$OUT/o.txt"
"$BIN" "$FIX/does_not_exist.pgm" "$OUT/o.txt" " .oX" > /dev/null 2>&1
rc=$?
if [ $rc -eq 1 ] && [ ! -e "$OUT/o.txt" ]; then ok missing_input_file
else bad missing_input_file "exit code $rc"; fi

# Wrong argument counts
"$BIN" > /dev/null 2>&1; rc1=$?
"$BIN" "$FIX/spaced.pgm" "$OUT/o.txt" > /dev/null 2>&1; rc2=$?
"$BIN" "$FIX/spaced.pgm" "$OUT/o.txt" " .oX" extra > /dev/null 2>&1; rc3=$?
if [ $rc1 -eq 1 ] && [ $rc2 -eq 1 ] && [ $rc3 -eq 1 ]; then ok wrong_argument_count
else bad wrong_argument_count "exit codes $rc1 $rc2 $rc3"; fi

printf 'pgmtoascii: %d passed, %d failed\n' "$pass" "$fail"
[ "$fail" -eq 0 ]
