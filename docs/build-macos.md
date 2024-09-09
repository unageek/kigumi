# Building on macOS Sonoma

## Prerequisites

- Xcode 15

- [Homebrew](https://brew.sh)

- Build tools

  ```bash
  brew install autoconf-archive automake cmake libtool llvm ninja
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

## [Using kigumi CLI](cli.md)
