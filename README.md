# ![kigumi](docs/logo.svg#gh-light-mode-only)![kigumi](docs/logo_dark.svg#gh-dark-mode-only)

Rock-solid Boolean operations on triangle meshes 🪨

## Build Instructions

[On Windows](docs/build-windows.md) • [On macOS](docs/build-macos.md) • [On Ubuntu](docs/build-ubuntu.md)

## Features

- Can handle non-manifold meshes
- Can handle open meshes
- Can handle meshes composed of disjoint parts
- Distinguish between the empty and the entire meshes
- Apply multiple Boolean operators simultaneously
- Attach custom data to faces that propagate through Boolean operations
- Save exact mesh data in a portable binary format

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

## Table of operators

| `Operator::`               | Set notation        | Venn diagram                              |
| -------------------------- | ------------------- | ----------------------------------------- |
| `V`                        | $U$ (the universe)  | <img width="100" src="docs/Venn1111.png"> |
| `A`, `Union`               | $A ∪ B$             | <img width="100" src="docs/Venn0111.png"> |
| `B`                        | $(B ⧵ A)^c$         | <img width="100" src="docs/Venn1101.png"> |
| `C`                        | $(A ⧵ B)^c$         | <img width="100" src="docs/Venn1011.png"> |
| `D`                        | $(A ∩ B)^c$         | <img width="100" src="docs/Venn1110.png"> |
| `E`                        | $(A △ B)^c$         | <img width="100" src="docs/Venn1001.png"> |
| `F`                        | $A^c$               | <img width="100" src="docs/Venn1010.png"> |
| `G`                        | $B^c$               | <img width="100" src="docs/Venn1100.png"> |
| `H`                        | $B$                 | <img width="100" src="docs/Venn0011.png"> |
| `I`                        | $A$                 | <img width="100" src="docs/Venn0101.png"> |
| `J`, `SymmetricDifference` | $A △ B$             | <img width="100" src="docs/Venn0110.png"> |
| `K`, `Intersection`        | $A ∩ B$             | <img width="100" src="docs/Venn0001.png"> |
| `L`, `Difference`          | $A ⧵ B$             | <img width="100" src="docs/Venn0100.png"> |
| `M`                        | $B ⧵ A$             | <img width="100" src="docs/Venn0010.png"> |
| `X`                        | $(A ∪ B)^c$         | <img width="100" src="docs/Venn1000.png"> |
| `O`                        | $∅$ (the empty set) | <img width="100" src="docs/Venn0000.png"> |

## Algorithm

An enhanced version of the algorithm described in [[1]](#1) is used.

## References

- <a id="1">[1]</a> Barki, H., Guennebaud, G., & Foufou, S. (2015). Exact, robust, and efficient regularized Booleans on general 3D meshes. _Computers & Mathematics With Applications_, _70_(6), 1235–1254. [https://doi.org/10.1016/j.camwa.2015.06.016](https://doi.org/10.1016/j.camwa.2015.06.016)
