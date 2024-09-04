# Building on Ubuntu 24.04 LTS

## Prerequisites

- Build tools

  ```bash
  sudo apt install clang cmake libomp-dev ninja-build
  ```

## Clone

```bash
git clone --recurse-submodules https://github.com/unageek/kigumi.git
cd kigumi
```

To update an existing repository:

```bash
git pull --recurse-submodules
```

## Build

```bash
export CC=clang
export CXX=clang++
./run configure  # or ./run c
./run build      # or ./run b
```

# [Using kigumi CLI](cli.md)
