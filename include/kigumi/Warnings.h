#pragma once

#include <cstdint>

namespace kigumi {

enum class Warnings : std::uint8_t {
  None = 0,
  FirstMeshPartiallyIntersectsWithSecondMesh = 1,
  SecondMeshPartiallyIntersectsWithFirstMesh = 2,
};

inline Warnings operator|(Warnings a, Warnings b) {
  return static_cast<Warnings>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

inline Warnings& operator|=(Warnings& a, Warnings b) { return a = a | b; }

inline Warnings operator&(Warnings a, Warnings b) {
  return static_cast<Warnings>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
}

inline Warnings& operator&=(Warnings& a, Warnings b) { return a = a & b; }

}  // namespace kigumi
