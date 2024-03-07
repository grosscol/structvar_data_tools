# Run from root directory.
# Convenience method for watching changes with entr:
#   fdfind '\.(cpp|hpp|in)$' src include test | entr -c ./build_and_grind.sh
cmake --build ./build
valgrind --leak-check=full --verbose --track-origins=yes --log-file=valgrind.out \
  ./het_hom_sel --file test/data/structvar_sample_input.vcf
