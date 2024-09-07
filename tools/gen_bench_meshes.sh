#!/usr/bin/env sh

set -eux

cd "$(dirname "$0")/.."

./build/cli/kigumi boolean meshes/cos_sin.obj meshes/box.obj --int meshes/cos_sin_closed.obj
./build/cli/kigumi boolean meshes/sin_cos.obj meshes/box.obj --int meshes/sin_cos_closed.obj
