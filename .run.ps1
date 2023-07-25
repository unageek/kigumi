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

$externalArgs = @()
$i = [array]::indexof($args, "--")
if ( $i -ne -1 ) {
    $__, $externalArgs = $args[$i..($args.length - 1)]
}

Set-Location $PSScriptRoot

switch -regex ($args[0]) {
    '^c(onfigure)$' {
        loadBuildEnvironment
        Exec { cmake -Bbuild -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE='vcpkg/scripts/buildsystems/vcpkg.cmake' $externalArgs }
        break
    }
    '^b(uild)?$' {
        loadBuildEnvironment
        Exec { cmake --build build }
        break
    }
    '^r(un)?$' {
        $bin = $args[1]
        Exec { & .\build\$bin $externalArgs }
        break
    }
    '^t(est)?$' {
        loadBuildEnvironment
        Exec { ctest -V --test-dir build }
        break
    }
    '^configure-on-ci$' {
        loadBuildEnvironment
        Exec { cmake -Bbuild -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE='C:/vcpkg/scripts/buildsystems/vcpkg.cmake' }
        break
    }
    default {
        throw "Unknown task: '$($args[0])'"
    }
}
