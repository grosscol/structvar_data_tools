# Het Hom Selector

## Development

### Requred -dev libraries
- zlib
- bzip2
- lzma
- libcurl
- openssl-crypto

### Build using CMAKE
1. Generate build system for project
    ```sh
    cmake -S . -B build/
    ```

2. Build the project
    ```sh
    cmake --build ./build
    ```

### Automatic build while developing

```sh
fdfind '\.(cpp|hpp|in)$' src include | entr -c cmake --build ./build
```

### Automatic build and test while developing

```sh
fdfind '\.(cpp|hpp|in)$' src include test | entr -c -s "cmake --build ./build; cd build; ctest"
```

