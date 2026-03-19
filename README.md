# ![kigumi](docs/logo.svg#gh-light-mode-only)![kigumi](docs/logo_dark.svg#gh-dark-mode-only)

**kigumi** is a header-only C++ library for performing regularized Boolean set operations on 3D regions that are (partially) bounded by triangular meshes.

With **kigumi**, you can, for example:

1. Compute the union of a terrain mesh and a water-surface mesh to simulate flooding.
2. Intersect that result with a box to produce a solid model ready for 3D printing.

## Features

- Perform Boolean operations between 3D regions (partially) bounded by triangular meshes.
- Meshes can be open or closed, manifold or non-manifold.
- Regions can be disjoint, [hollow](https://en.wikipedia.org/wiki/Spherical_shell), or even nested.
- Correctly handle special regions: the empty set and the universe.
- Apply multiple Boolean operators simultaneously.
- Attach custom data to mesh faces that propagate through Boolean operations.
- Save regions without losing information in a portable binary format.

## Benchmarks

Here are the timings (in seconds) for computing the Boolean intersection between two meshes, excluding I/O time.

| Test case         | [coref.][coref] (seq.) | [geogram][geogram] (par.) | kigumi (seq.)¹ | kigumi (par.) | [libigl][libigl] (par.)² | [manif.][manif] (seq.)³ | manif. (par.) | [MCUT][mcut] (par.) |
| ----------------- | ---------------------: | ------------------------: | -------------: | ------------: | -----------------------: | ----------------------: | ------------: | ------------------: |
| **Open**          |                    3.2 |                    FAILED |            2.4 |           0.6 |                   FAILED |                  FAILED |        FAILED |              FAILED |
| **Open & closed** |                 FAILED |                       5.5 |            1.0 |           0.4 |                   FAILED |                  FAILED |        FAILED |              FAILED |
| **Closed**        |                   27.2 |                    FAILED |            4.6 |           1.1 |                     39.3 |                     1.3 |           0.2 |              FAILED |
| **Non-manifold**  |                 FAILED |                    FAILED |            0.5 |           0.1 |                   FAILED |                  FAILED |        FAILED |              FAILED |
| **Exactness**     |                  Exact |                     Exact |          Exact |         Exact |                    Exact |                 Approx. |       Approx. |             Approx. |

¹ Ran with `KIGUMI_NUM_THREADS=1`. ² `igl::copyleft::cgal::mesh_boolean` with `CGAL::Lazy_exact_nt<mpq_class>` as the number type was used. ³ Configured with `-DMANIFOLD_PAR=OFF`.

Benchmarks were performed on a MacBook Pro (14-inch, M4, 2024) with a 16-core CPU. Programs were built with Homebrew Clang 22.1.1. Spotlight was disabled during benchmarking. The following commands were used:

```
./tools/gen_bench_meshes.sh
./tools/run_benches.sh
```

## Documentation

API

- [Triangle_soup.h](include/kigumi/Triangle_soup.h)
- [Region.h](include/kigumi/Region.h)
- [Boolean_region_builder.h](include/kigumi/Boolean_region_builder.h).
- [Boolean_operator.h](include/kigumi/Boolean_operator.h)
- [Warnings.h](include/kigumi/Warnings.h)
- [Table of Boolean operators](docs/boolean-operators.md)

The kigumi CLI

- Build instructions
  - [Windows](docs/build-windows.md) • [macOS](docs/build-macos.md) • [Ubuntu](docs/build-ubuntu.md)
- [Using the kigumi CLI](docs/cli.md)

## Requirements on input meshes

Boundary meshes must satisfy the following conditions for Boolean operations to work properly. If any of these
conditions are not met, the result is undefined and may emit warnings, crash, or produce broken meshes.

1. Meshes must not have degenerate (zero-area) faces.
1. Meshes must not self-intersect, i.e., every pair of distinct faces must meet one of the following conditions:
   - They share an edge but do not intersect elsewhere.
   - They share a vertex but do not intersect elsewhere.
   - They do not intersect at all.
1. Open meshes must be clipped with a common convex region.
1. Faces that have more than three vertices should be interpreted as triangle fans.

## Algorithm

An enhanced and optimized version of the algorithm described in the following paper is used:

- Barki, H., Guennebaud, G., & Foufou, S. (2015). Exact, robust, and efficient regularized Booleans on general 3D meshes. _Computers & Mathematics With Applications_, _70_(6), 1235–1254. [https://doi.org/10.1016/j.camwa.2015.06.016](https://doi.org/10.1016/j.camwa.2015.06.016)

## License

The code is available under the [MIT License](LICENSE).

To use **kigumi** in your application, you need a library that implements an interface compatible with a subset of CGAL. The following CGAL packages and their dependencies are required:

| Package                              | License           |
| ------------------------------------ | ----------------- |
| **2D and 3D Linear Geometry Kernel** | LGPL              |
| **2D Triangulations**                | GPL or commercial |

---

![Green Computing](docs/green_computing.svg#gh-light-mode-only)![Green Computing](docs/green_computing_dark.svg#gh-dark-mode-only)

[Adopting highly optimized algorithms helps reduce CO₂ emissions.](https://en.wikipedia.org/wiki/Green_computing)

[coref]: https://doc.cgal.org/latest/Polygon_mesh_processing/index.html#Coref_section
[geogram]: https://github.com/BrunoLevy/geogram
[libigl]: https://github.com/libigl/libigl
[manif]: https://github.com/elalish/manifold
[mcut]: https://github.com/cutdigital/mcut
