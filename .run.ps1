Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

. .\tools\Exec.ps1
. .\tools\Invoke-BatchFile.ps1

function loadBuildEnvironment {
    $vswhere = Resolve-Path "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

    $vsDir = & $vswhere -latest `
        -requires Microsoft.VisualStudio.Workload.NativeDesktop `
        -property installationPath

    if (-not $vsDir) {
        throw 'MSVC is not installed.'
    }

    Invoke-BatchFile "$vsDir\VC\Auxiliary\Build\vcvars64.bat"
}

if ($args.Length -lt 1) {
    throw 'No task is specified.'
}

Set-Location $PSScriptRoot

switch -regex ($args[0]) {
    '^c(onfigure)?$' {
        loadBuildEnvironment
        Exec { cmake -Bbuild -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE='vcpkg/scripts/buildsystems/vcpkg.cmake' }
        break
    }
    '^b(uild)?$' {
        loadBuildEnvironment
        Exec { cmake --build build }
        break
    }
    '^t(est)?$' {
        loadBuildEnvironment
        Exec { ctest -V --test-dir build }
        break
    }
    default {
        throw "Unknown task: '$($args[0])'"
    }
}
