# Building on macOS

## Prerequisites

- Xcode Command Line Tools

  ```bash
  xcode-select --install
  ```

- [Homebrew](https://brew.sh)

- Tools and libraries

  ```bash
  brew install automake autoconf-archive cmake llvm ninja
  ```

## Clone

```
git clone https://github.com/unageek/kigumi.git
```

## Build

```
cd kigumi
./run configure  # or ./run c
./run build      # or ./run b
```

## Run

See [Running example programs](run.md).
