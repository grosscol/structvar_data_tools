# Run from root directory.
# Convenience method for watching changes with entr:
#   fdfind '\.(cpp|hpp|in)$' src include test | entr -c ./build_and_grind.sh
set -e
cmake --build ./build


INPUT_DATA=het_hom_selector/test/data/structvar_sample_input.vcf
BINARY=build/het_hom_selector/bin/het_hom_sel

# Positional args for random sample of file input
valgrind --show-leak-kinds=definite,indirect,possible --leak-check=full \
  --verbose --track-origins=yes --log-file=/tmp/valgrind.rnd.out \
  ${BINARY} rnd ${INPUT_DATA} > /tmp/hhs_rnd.out 2> /tmp/hhs_rnd.err

# Positional args for all samples and variant ids of file input
valgrind --show-leak-kinds=definite,indirect,possible --leak-check=full \
  --verbose --track-origins=yes --log-file=/tmp/valgrind.all.out \
  ${BINARY} --emit-id all ${INPUT_DATA} > /tmp/hhs_all.out 2> /tmp/hhs_all.err

# Input via stdin
valgrind --show-leak-kinds=definite,indirect,possible --leak-check=full \
  --verbose --track-origins=yes --log-file=/tmp/valgrind.rnd.stdin.out \
  ${BINARY} rnd < ${INPUT_DATA} > /tmp/hhs_rnd.stdin.out 2> /tmp/hhs_rnd.stdin.err

# Positional args for bad input
valgrind --show-leak-kinds=definite,indirect,possible --leak-check=full \
  --verbose --track-origins=yes --log-file=/tmp/valgrind.nonzero.out \
  ${BINARY} rnd test/data/no_such_file.vcf > /tmp/nonzero.out 2> /tmp/nonzero.err

echo "Running Valgrind"

echo "Rnd Samples Output:"
tail /tmp/hhs_rnd.out
echo ""
echo "All Samples Output:"
tail /tmp/hhs_all.out

echo ""
echo "------ VALGRIND: RND ----------------------"
echo -n "|"
grep -A4 'LEAK SUMMARY' < /tmp/valgrind.rnd.out | grep -o 'lost:.*bytes' | tr '\n' '|'
echo ""

echo "------ VALGRIND: ALL ----------------------"
echo -n "|"
grep -A4 'LEAK SUMMARY' < /tmp/valgrind.all.out | grep -o 'lost:.*bytes' | tr '\n' '|'
echo ""

echo "------ VALGRIND: RND STDIN ----------------"
echo -n "|"
grep -A4 'LEAK SUMMARY' < /tmp/valgrind.rnd.stdin.out | grep -o 'lost:.*bytes' | tr '\n' '|'
echo ""

echo "------ VALGRIND: EXIT 1 -------------------"
echo -n "|"
grep -A4 'LEAK SUMMARY' < /tmp/valgrind.nonzero.out | grep -o 'lost:.*bytes' | tr '\n' '|'
echo ""
echo "-------------------------------------------"
