# Building boole on macOS

## Prerequisites

- Xcode Command Line Tools

  ```bash
  xcode-select --install
  ```

- [Homebrew](https://brew.sh)

- Tools and libraries

  ```bash
  brew install cgal cmake llvm ninja
  ```

## Clone

```
git clone https://github.com/unageek/boole.git
```

## Build

```
cd boole
./run cmake -- -DCMAKE_CXX_COMPILER=$(brew --prefix)/opt/llvm/bin/clang++
./run b
```

## Run

```
./run r src/boole -- a.obj b.obj
```

The output meshes are saved as out_inter.obj and out_union.obj.in the current directory.
