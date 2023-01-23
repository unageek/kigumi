# Building on macOS

## Prerequisites

- Xcode Command Line Tools

  ```bash
  xcode-select --install
  ```

- [Homebrew](https://brew.sh)

- Tools and libraries

  ```bash
  brew install cgal cmake googletest llvm ninja
  ```

## Clone

```
git clone https://github.com/unageek/kigumi.git
```

## Build

```
cd kigumi
./run cmake -- -DCMAKE_CXX_COMPILER=$(brew --prefix)/opt/llvm/bin/clang++
./run b
```

## Run

See [Running example programs](run.md).
