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
export CC=$(brew --prefix)/opt/llvm/bin/clang
export CXX=$(brew --prefix)/opt/llvm/bin/clang++
./run configure  # or ./run c
./run build      # or ./run b
```

## [Running example programs](run.md)
