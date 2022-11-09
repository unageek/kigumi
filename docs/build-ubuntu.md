# Building boole On Ubuntu 22.04

## Prerequisites

```bash
sudo apt install clang cmake libcgal-dev libomp-dev ninja-build zsh
```

## Clone

```
git clone https://github.com/unageek/boole.git
```

## Build

```
cd boole
./run cmake
./run b
```

### Run

```
./run r src/boole -- a.obj b.obj
```

The output meshes are saved as out_inter.obj and out_union.obj.in the current directory.
