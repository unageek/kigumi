#pragma once

namespace kigumi {

enum class Warnings {
  None = 0,
  FirstMeshPartiallyIntersectsWithSecondMesh = 1,
  SecondMeshPartiallyIntersectsWithFirstMesh = 2,
};

inline Warnings operator|(Warnings a, Warnings b) {
  return static_cast<Warnings>(static_cast<int>(a) | static_cast<int>(b));
}

inline Warnings& operator|=(Warnings& a, Warnings b) { return a = a | b; }

inline Warnings operator&(Warnings a, Warnings b) {
  return static_cast<Warnings>(static_cast<int>(a) & static_cast<int>(b));
}

inline Warnings& operator&=(Warnings& a, Warnings b) { return a = a & b; }

}  // namespace kigumi