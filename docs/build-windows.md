# Building on Windows

## Prerequisites

1. [Visual Studio 2022](https://visualstudio.microsoft.com/)

   Under **Workloads**, select the following item:

   - **Desktop & Mobile**

     - **Desktop development with C++**

   Under **Individual components**, select the following item:

   - **Code tools**

     - **Git for Windows**

## Clone

```
git clone --recursive https://github.com/unageek/kigumi.git
```

## Build

```
cd kigumi
./run configure  # or ./run c
./run build      # or ./run b
```

## Run

See [Running example programs](run.md).
