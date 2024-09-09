#!/usr/bin/env sh

set -eux

cd "$(dirname "$0")/.."

./build/cli/kigumi boolean meshes/cos_sin.obj meshes/box.obj --int meshes/cos_sin_closed.obj
./build/cli/kigumi boolean meshes/sin_cos.obj meshes/box.obj --int meshes/sin_cos_closed.obj
./build/cli/kigumi boolean meshes/checker.obj meshes/text.obj --int meshes/checker_text.obj
./build/cli/kigumi boolean :full: meshes/checker_text.obj --dif meshes/inv_checker_text.obj
