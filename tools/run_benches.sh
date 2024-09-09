#!/usr/bin/env sh

set -eux

cd "$(dirname "$0")/.."

set +e

while read -r method
do
    # Open
    "./build/benches/$method/$method" meshes/cos_sin.obj meshes/sin_cos.obj "meshes/oo_$method.obj"
    # Open & closed
    "./build/benches/$method/$method" meshes/cos_sin.obj meshes/box.obj "meshes/oc_$method.obj"
    # Closed
    "./build/benches/$method/$method" meshes/cos_sin_closed.obj meshes/sin_cos_closed.obj "meshes/cc_$method.obj"
    # Non-manifold
    "./build/benches/$method/$method" meshes/plate.obj meshes/inv_checker_text.obj "meshes/nonmanif_$method.obj"
done < tools/bench_methods.txt
