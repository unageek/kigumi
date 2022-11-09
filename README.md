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
1. Mesh faces must be oriented consistently.
1. A mesh must not self-intersect, i.e., the interior of a face must not intersect with the interior of another face.
1. There must exist a closed convex subset of $ℝ^3$, $\mathcal{R}$, such that $∀M ∈ \\{M_1, M_2\\} : M ⊂ \mathcal{R} ∧ ∂M = M ∩ ∂\mathcal{R}$, where $M_1$ and $M_2$ are input meshes.

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
