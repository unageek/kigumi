#!/usr/bin/env sh

set -eux

cd "$(dirname "$0")/.."

set +e

while read -r method
do
    "./build/benches/$method/$method" meshes/cos_sin.obj meshes/sin_cos.obj "meshes/oo_$method.obj"
    "./build/benches/$method/$method" meshes/cos_sin.obj meshes/box.obj "meshes/oc_$method.obj"
    "./build/benches/$method/$method" meshes/cos_sin_closed.obj meshes/sin_cos_closed.obj "meshes/cc_$method.obj"
done < tools/bench_methods.txt
