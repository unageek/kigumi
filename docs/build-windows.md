# Building on Windows 10/11

## Prerequisites

1. [Visual Studio 2022](https://visualstudio.microsoft.com/)

   Under **Workloads**, select the following item:

   - **Desktop & Mobile**

     - **Desktop development with C++**

   Under **Individual components**, select the following item:

   - **Code tools**

     - **Git for Windows**

## Clone

```ps1
git clone --recurse-submodules https://github.com/unageek/kigumi.git
cd kigumi
```

To update an existing repository:

```ps1
git pull --recurse-submodules
```

## Build

```ps1
./run configure  # or ./run c
./run build      # or ./run b
```

# [Running example programs](run.md)
