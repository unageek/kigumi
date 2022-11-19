# boole

3D regularized Boolean set operations that just work! 🙌

## Build Instructions

[On Windows](docs/build-windows.md) • [On macOS](docs/build-macos.md) • [On Ubuntu](docs/build-ubuntu.md)

## Features

- Can handle non-manifold meshes
- Can handle open meshes
- Can handle disjointness

## Limitations

The following conditions must be satisfied so that Boolean operations work properly. If some are not met, the result is undefined (operations can result in an error, crash or fail silently).

1. A mesh must not be empty.
   - An empty mesh can represent either the empty set or the whole space. Thus, the program refuses to handle such inputs.
1. A mesh must only contain triangle faces.
1. A mesh must not have degenerate (zero-area) faces.
1. For each pair of faces (f1, f2) (f1 ≠ f2), one of the following must be met:
   - f1 and f2 have an edge in common and do not intersect elsewhere than the edge,
   - f1 and f2 have a vertex in common and do not intersect elsewhere than the vertex,
   - f1 and f2 do not intersect.
1. Let $M_1, M_2$ be the input meshes. There must exist closed sets $\mathcal{M}_1, \mathcal{M}_2 ⊂ ℝ^3$, and a closed convex set $\mathcal{R} ⊂ ℝ^3$ that satisfies the following conditions:
   - $M_1 = ∂\mathcal{M}_1 ∩ \mathcal{R}$,
   - $M_2 = ∂\mathcal{M}_2 ∩ \mathcal{R}$,
   - each face of $M_1$ is oriented so that the normal points toward the exterior of $\mathcal{M}_1$,
   - each face of $M_2$ is oriented so that the normal points toward the exterior of $\mathcal{M}_2$.

## TODO

- Unit testing
- Load/save exact meshes
- Attach custom data to faces
- Optimization for equivalent/complementary inputs
- Mesh repairing

## Algorithm

The algorithm described in [[1]](#1) is implemented.

## References

- <a id="1">[1]</a> Barki, H., Guennebaud, G., & Foufou, S. (2015). Exact, robust, and efficient regularized Booleans on general 3D meshes. _Computers & Mathematics With Applications_, _70_(6), 1235–1254. [https://doi.org/10.1016/j.camwa.2015.06.016](https://doi.org/10.1016/j.camwa.2015.06.016)
