# Building boole on Windows

## Prerequisites

1. [PowerShell](https://apps.microsoft.com/store/detail/powershell/9MZ1SNWT0N5D)

1. [Visual Studio 2022](https://visualstudio.microsoft.com/)

   Under **Workloads**, select the following item:

   - **Desktop & Mobile**

     - **Desktop development with C++**

   Under **Individual components**, select the following item:

   - **Code tools**

     - **Git for Windows**

## Clone

```
git clone --recursive https://github.com/unageek/boole.git
```

## Build

```
cd boole
./run init-vcpkg
./run cmake
./run b
```

### Run

```
./run r src/boole -- a.obj b.obj
```

The output meshes are saved as out_inter.obj and out_union.obj.in the current directory.
