#!/usr/bin/env bash
# Run from root directory.
# Convenience method for watching changes with entr:
#   fdfind '\.(cpp|hpp|in)$' src include test | entr -c ./build_and_grind.sh
set -e
cmake --build ./build


HHS_DATA=het_hom_selector/test/data/structvar_sample_input.vcf
HHS_BIN=build/het_hom_selector/bin/het_hom_sel

echo "Het Home Selector Valgrind"

# Positional args for random sample of file input
valgrind --show-leak-kinds=definite,indirect,possible --leak-check=full \
  --verbose --track-origins=yes --log-file=/tmp/vg.hhs.rnd.out \
  ${HHS_BIN} rnd ${HHS_DATA} > /tmp/hhs_rnd.out 2> /tmp/hhs_rnd.err

# Positional args for all samples and variant ids of file input
valgrind --show-leak-kinds=definite,indirect,possible --leak-check=full \
  --verbose --track-origins=yes --log-file=/tmp/vg.hhs.all.out \
  ${HHS_BIN} --emit-id all ${HHS_DATA} > /tmp/hhs_all.out 2> /tmp/hhs_all.err

# Input via stdin
valgrind --show-leak-kinds=definite,indirect,possible --leak-check=full \
  --verbose --track-origins=yes --log-file=/tmp/vg.hhs.rnd.stdin.out \
  ${HHS_BIN} rnd < ${HHS_DATA} > /tmp/hhs_rnd.stdin.out 2> /tmp/hhs_rnd.stdin.err

# Positional args for bad input
valgrind --show-leak-kinds=definite,indirect,possible --leak-check=full \
  --verbose --track-origins=yes --log-file=/tmp/vg.hhs.nonzero.out \
  ${HHS_BIN} rnd test/data/no_such_file.vcf > /tmp/nonzero.out 2> /tmp/nonzero.err


echo ""
echo "------ VALGRIND: RND ----------------------"
echo -n "|"
grep -A4 'LEAK SUMMARY' < /tmp/vg.hhs.rnd.out | grep -o 'lost:.*bytes' | tr '\n' '|'
echo ""

echo "------ VALGRIND: ALL ----------------------"
echo -n "|"
grep -A4 'LEAK SUMMARY' < /tmp/vg.hhs.all.out | grep -o 'lost:.*bytes' | tr '\n' '|'
echo ""

echo "------ VALGRIND: RND STDIN ----------------"
echo -n "|"
grep -A4 'LEAK SUMMARY' < /tmp/vg.hhs.rnd.stdin.out | grep -o 'lost:.*bytes' | tr '\n' '|'
echo ""

echo "------ VALGRIND: EXIT 1 -------------------"
echo -n "|"
grep -A4 'LEAK SUMMARY' < /tmp/vg.hhs.nonzero.out | grep -o 'lost:.*bytes' | tr '\n' '|'
echo ""
echo "-------------------------------------------"


echo "Read Summarizer Valgrind"

RS_DATA=cram_summarizer/test/data/del_1_sample_1.sam
RS_BIN=build/cram_summarizer/bin/read_summ

# Positional args for random sample of file input
valgrind --show-leak-kinds=definite,indirect,possible --leak-check=full \
  --verbose --track-origins=yes --log-file=/tmp/vg.rs.rnd.out \
  ${RS_BIN} ${RS_DATA} > /tmp/rs_rnd.out 2> /tmp/rs_rnd.err

echo ""
echo "------ VALGRIND: SAM ----------------------"
echo -n "|"
grep -A4 'LEAK SUMMARY' < /tmp/vg.rs.rnd.out | grep -o 'lost:.*bytes' | tr '\n' '|'
grep 'All heap blocks were freed' < /tmp/vg.rs.rnd.out | cut --complement -d ' ' -f 1
echo ""
echo "-------------------------------------------"
