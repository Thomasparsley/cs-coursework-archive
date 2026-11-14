#!/bin/sh
# Regenerates the PGM fixtures (the CRLF one needs real CR bytes).
set -e
cd "$(dirname "$0")/fixtures"

printf 'P2\n3 2\n3\n0 1 2\n3 2 1\n' > spaced.pgm
printf 'P2\n3 2\n3\n012\n321\n' > unspaced.pgm
printf 'P2\n3 2\n3\n0 1 2 3 2 1\n' > oneline.pgm
printf 'P2\n# header comment\n3 2 # inline\n# another\n3\n0 1 # data comment\n2\n# full line\n3 2 1\n' > comments.pgm
printf 'P2\r\n# c\r\n3 2\r\n3\r\n0 1 2\r\n3 2 1\r\n' > crlf.pgm
printf 'P2\n1 1\n1\n1\n' > one_by_one.pgm
printf 'P2\n3 2\n15\n10 15 0\n7 12 9\n' > multidigit.pgm
printf 'P2\n2 1\n10\n10 0' > multidigit_nonl.pgm

printf 'P2\n3 2\n3\n0 1 2\n3 2\n' > err_too_few.pgm
printf 'P2\n3 2\n3\n0 1 2\n3 2 1 0\n' > err_too_many.pgm
printf 'P2\n3 2\n3\n0 1 2\n3 -1 1\n' > err_negative.pgm
printf 'P2\n3 2\n3\n0 1 2\n3 9 1\n' > err_over_maxval.pgm
printf 'P2\n3 2\n15\n0 1 2\n3 16 1\n' > err_over_maxval_multi.pgm
printf 'P2\n3 2\n3\n0 1 2\n3 2 x\n' > err_bad_char.pgm
printf 'P5\n3 2\n3\n0 1 2\n3 2 1\n' > err_marker.pgm
printf 'P2\n3\n' > err_header_incomplete.pgm
printf 'P2\n0 2\n3\n' > err_zero_width.pgm
: > err_empty.pgm

cp ../../input.pgm sample.pgm

printf ' .o\nXo.\n' > spaced.expected
cp spaced.expected unspaced.expected
cp spaced.expected comments.expected
cp spaced.expected crlf.expected
cp spaced.expected oneline.expected
printf 'af0\n7c9\n' > multidigit.expected
printf 'a0\n' > multidigit_nonl.expected
printf '#\n' > one_by_one.expected
