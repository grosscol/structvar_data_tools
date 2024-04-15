# Structral Variant Data Tools

Tooling to summarize data from structural variants bcf and cram files.

## Requred -dev libraries
- zlib
- bzip2
- lzma
- libcurl
- openssl-crypto

### Build all projects using CMAKE
1. Generate build system for project
    ```sh
    cmake -S . -B build/
    ```

2. Build the project
    ```sh
    cmake --build ./build
    ```

## Het Hom Selector
Extract all or a random subset of heterozygous and homozygous sample IDs from a vcf.

## Development

### Automatic build while developing

```sh
fdfind '\.(cpp|hpp|in)$' . | entr -c cmake --build ./build
```

### Automatic build and test while developing

```sh
fdfind '\.(cpp|hpp|in)$' . | entr -c -s "cmake --build ./build; cd build; ctest"
```

or use the convenience script `./build_and_test.sh`
```sh
fdfind '\.(cpp|hpp|in)$' . | entr -c ./build_and_test.sh
```

### Automatic Checking for memory leaks with Valgrind
Note: If doing a full check, need to suppressing still reachable blocks due to [sync\_with\_stdio(false)](https://www.mail-archive.com/gcc-bugs@gcc.gnu.org/msg160316.html)

```sh
echo "build/het_hom_selector/bin/het_hom_sel" | entr -c ./build_and_grind.sh
```


