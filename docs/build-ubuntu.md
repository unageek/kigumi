# Building on Ubuntu 24.04 LTS

## Prerequisites

- Build tools

  ```bash
  sudo apt install autoconf clang cmake curl git libomp-dev libtool ninja-build unzip zip
  ```

  To build the benchmarks, you also need `xorg-dev`, which is required by geogram:

  ```bash
  sudo apt install xorg-dev
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
./run configure  # or ./run c
./run build      # or ./run b
```

# [Using kigumi CLI](cli.md)
