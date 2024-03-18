# Run from root directory.
# Convenience method for watching changes with entr:
#   fdfind '\.(cpp|hpp|in)$' src include test | entr -c ./build_and_grind.sh
set -e
cmake --build ./build

valgrind --show-leak-kinds=definite,indirect,possible --leak-check=full \
  --verbose --track-origins=yes --log-file=valgrind.zero.out --suppressions=./valgrind.supp \
  ./het_hom_sel --file test/data/structvar_sample_input.vcf > /tmp/zero.out 2> /tmp/zero.err

valgrind --show-leak-kinds=definite,indirect,possible --leak-check=full \
  --verbose --track-origins=yes --log-file=valgrind.zero.stdin.out --suppressions=./valgrind.supp \
  ./het_hom_sel < test/data/structvar_sample_input.vcf > /tmp/zero.stdin.out 2> /tmp/zero.stdin.err

valgrind --show-leak-kinds=definite,indirect,possible --leak-check=full \
  --verbose --track-origins=yes --log-file=valgrind.nonzero.out --suppressions=./valgrind.supp \
  ./het_hom_sel --file test/data/no_such_file.vcf > /tmp/nonzero.out 2> /tmp/nonzero.err 

echo "Running Valgrind"

cat /tmp/zero.out

echo ""
echo "------ VALGRIND: EXIT 0 -------------------"
echo -n "|"
grep -A4 'LEAK SUMMARY' < valgrind.zero.out | grep -o 'lost:.*bytes' | tr '\n' '|'
echo ""

echo "------ VALGRIND: STDIN  -------------------"
echo -n "|"
grep -A4 'LEAK SUMMARY' < valgrind.zero.stdin.out | grep -o 'lost:.*bytes' | tr '\n' '|'
echo ""

echo "------ VALGRIND: EXIT 1 -------------------"
echo -n "|"
grep -A4 'LEAK SUMMARY' < valgrind.nonzero.out | grep -o 'lost:.*bytes' | tr '\n' '|'
echo ""
echo "-------------------------------------------"
