#!/usr/bin/env sh

set -eux

cd "$(dirname "$0")/.."

set +e

./build/benches/corefinement/corefinement
./build/benches/corefinement/corefinement "$1" "$2" meshes/int_corefinement.obj

./build/benches/geogram/geogram
NUM_THREADS=1 ./build/benches/geogram/geogram "$1" "$2" meshes/int_geogram.obj
./build/benches/geogram/geogram "$1" "$2" meshes/int_geogram.obj

./build/benches/kigumi/kigumi
NUM_THREADS=1 ./build/benches/kigumi/kigumi "$1" "$2" meshes/int_kigumi.obj
./build/benches/kigumi/kigumi "$1" "$2" meshes/int_kigumi.obj

./build/benches/manifold/manifold
NUM_THREADS=1 ./build/benches/manifold/manifold "$1" "$2" meshes/int_manifold.obj
./build/benches/manifold/manifold "$1" "$2" meshes/int_manifold.obj
