#pragma once

#include <cstdint>
#include <stdexcept>
#include <tuple>

namespace kigumi {

enum class TriangleRegion : std::uint8_t {
  LeftVertex0 = 1,
  LeftVertex1 = 2,
  LeftVertex2 = 4,
  LeftEdge01 = LeftVertex0 | LeftVertex1,
  LeftEdge12 = LeftVertex1 | LeftVertex2,
  LeftEdge20 = LeftVertex2 | LeftVertex0,
  LeftFace = LeftVertex0 | LeftVertex1 | LeftVertex2,
  RightVertex0 = 8,
  RightVertex1 = 16,
  RightVertex2 = 32,
  RightEdge01 = RightVertex0 | RightVertex1,
  RightEdge12 = RightVertex1 | RightVertex2,
  RightEdge20 = RightVertex2 | RightVertex0,
  RightFace = RightVertex0 | RightVertex1 | RightVertex2,
};

inline TriangleRegion convex_hull(TriangleRegion r1, TriangleRegion r2) {
  return static_cast<TriangleRegion>(static_cast<std::uint8_t>(r1) | static_cast<std::uint8_t>(r2));
}

inline TriangleRegion intersection(TriangleRegion r1, TriangleRegion r2) {
  return static_cast<TriangleRegion>(static_cast<std::uint8_t>(r1) & static_cast<std::uint8_t>(r2));
}

inline std::tuple<TriangleRegion, TriangleRegion> edge_vertices(TriangleRegion edge) {
  switch (edge) {
    case TriangleRegion::LeftEdge01:
      return {TriangleRegion::LeftVertex0, TriangleRegion::LeftVertex1};
    case TriangleRegion::LeftEdge12:
      return {TriangleRegion::LeftVertex1, TriangleRegion::LeftVertex2};
    case TriangleRegion::LeftEdge20:
      return {TriangleRegion::LeftVertex2, TriangleRegion::LeftVertex0};
    case TriangleRegion::RightEdge01:
      return {TriangleRegion::RightVertex0, TriangleRegion::RightVertex1};
    case TriangleRegion::RightEdge12:
      return {TriangleRegion::RightVertex1, TriangleRegion::RightVertex2};
    case TriangleRegion::RightEdge20:
      return {TriangleRegion::RightVertex2, TriangleRegion::RightVertex0};
    default:
      throw std::invalid_argument("the region must be an edge");
  }
}

inline std::tuple<TriangleRegion, TriangleRegion, TriangleRegion> face_edges(TriangleRegion face) {
  switch (face) {
    case TriangleRegion::LeftFace:
      return {TriangleRegion::LeftEdge01, TriangleRegion::LeftEdge12, TriangleRegion::LeftEdge20};
    case TriangleRegion::RightFace:
      return {TriangleRegion::RightEdge01, TriangleRegion::RightEdge12,
              TriangleRegion::RightEdge20};
    default:
      throw std::invalid_argument("the region must be a face");
  }
}

inline std::tuple<TriangleRegion, TriangleRegion, TriangleRegion> face_vertices(
    TriangleRegion face) {
  switch (face) {
    case TriangleRegion::LeftFace:
      return {TriangleRegion::LeftVertex0, TriangleRegion::LeftVertex1,
              TriangleRegion::LeftVertex2};
    case TriangleRegion::RightFace:
      return {TriangleRegion::RightVertex0, TriangleRegion::RightVertex1,
              TriangleRegion::RightVertex2};
    default:
      throw std::invalid_argument("the region must be a face");
  }
}

inline int dimension(TriangleRegion region) {
  switch (region) {
    case TriangleRegion::LeftVertex0:
    case TriangleRegion::LeftVertex1:
    case TriangleRegion::LeftVertex2:
    case TriangleRegion::RightVertex0:
    case TriangleRegion::RightVertex1:
    case TriangleRegion::RightVertex2:
      return 0;
    case TriangleRegion::LeftEdge01:
    case TriangleRegion::LeftEdge12:
    case TriangleRegion::LeftEdge20:
    case TriangleRegion::RightEdge01:
    case TriangleRegion::RightEdge12:
    case TriangleRegion::RightEdge20:
      return 1;
    case TriangleRegion::LeftFace:
    case TriangleRegion::RightFace:
      return 2;
    default:
      throw std::invalid_argument("the region must be a simplex");
  }
}

inline bool is_left_region(TriangleRegion region) {
  return convex_hull(region, TriangleRegion::LeftFace) == TriangleRegion::LeftFace;
}

}  // namespace kigumi
