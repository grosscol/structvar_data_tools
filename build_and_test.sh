# Run from root directory.
# Convenience method for watching and testing changes with entr:
#   fdfind '\.(cpp|hpp|in)$' src include test | entr -c ./build_and_test.sh
set -e
cmake --build ./build
cd build
ctest --output-on-failure
