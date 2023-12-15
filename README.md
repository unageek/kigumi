# ![kigumi](docs/logo.svg#gh-light-mode-only)![kigumi](docs/logo_dark.svg#gh-dark-mode-only)

3D regularized Boolean set operations that just work! 🙌

## Build Instructions

[On Windows](docs/build-windows.md) • [On macOS](docs/build-macos.md) • [On Ubuntu](docs/build-ubuntu.md)

## Features

- Can handle non-manifold meshes
- Can handle open meshes
- Can handle disjointness
- Distinguish between the empty and the entire meshes
- Apply multiple Boolean operators simultaneously
- Attach custom data to faces that propagate through Boolean operations
- Save/load exact mesh data
  - Beware that the format is non-portable at the moment.

For details of the API, see [Kigumi_mesh.h](include/kigumi/Kigumi_mesh.h).

## Preconditions

The following conditions must be satisfied so that Boolean operations work properly. If some are not met, the result is undefined (operations can result in an error, crash or fail silently).

### Conditions on the input meshes

1. A mesh must not be empty.
   - An empty mesh can represent either the empty set or the whole space. Thus, the program refuses to handle such inputs.
1. A mesh must not have a degenerate (zero-area) face.
1. A mesh must not self-intersect.
   - In precise, for each pair of faces (f1, f2) (f1 ≠ f2), one of the following must be met:
     - f1 and f2 have an edge in common and do not intersect elsewhere than the edge,
     - f1 and f2 have a vertex in common and do not intersect elsewhere than the vertex,
     - f1 and f2 do not intersect.
1. If the meshes are not closed, they must be clipped with a common convex boundary.

Additional notes:

1. Faces that have more than three vertices are interpreted as triangle fans.

## Algorithm

The algorithm described in [[1]](#1) is implemented.

## References

- <a id="1">[1]</a> Barki, H., Guennebaud, G., & Foufou, S. (2015). Exact, robust, and efficient regularized Booleans on general 3D meshes. _Computers & Mathematics With Applications_, _70_(6), 1235–1254. [https://doi.org/10.1016/j.camwa.2015.06.016](https://doi.org/10.1016/j.camwa.2015.06.016)
