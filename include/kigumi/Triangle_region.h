#pragma once

#include <cstdint>
#include <stdexcept>
#include <tuple>

namespace kigumi {

enum class Triangle_region : std::uint8_t {
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

inline constexpr Triangle_region convex_hull(Triangle_region r1, Triangle_region r2) {
  return static_cast<Triangle_region>(static_cast<std::uint8_t>(r1) |
                                      static_cast<std::uint8_t>(r2));
}

inline constexpr Triangle_region intersection(Triangle_region r1, Triangle_region r2) {
  return static_cast<Triangle_region>(static_cast<std::uint8_t>(r1) &
                                      static_cast<std::uint8_t>(r2));
}

inline std::tuple<Triangle_region, Triangle_region> edge_vertices(Triangle_region edge) {
  switch (edge) {
    case Triangle_region::LeftEdge01:
      return {Triangle_region::LeftVertex0, Triangle_region::LeftVertex1};
    case Triangle_region::LeftEdge12:
      return {Triangle_region::LeftVertex1, Triangle_region::LeftVertex2};
    case Triangle_region::LeftEdge20:
      return {Triangle_region::LeftVertex2, Triangle_region::LeftVertex0};
    case Triangle_region::RightEdge01:
      return {Triangle_region::RightVertex0, Triangle_region::RightVertex1};
    case Triangle_region::RightEdge12:
      return {Triangle_region::RightVertex1, Triangle_region::RightVertex2};
    case Triangle_region::RightEdge20:
      return {Triangle_region::RightVertex2, Triangle_region::RightVertex0};
    default:
      throw std::invalid_argument("the region must be an edge");
  }
}

inline std::tuple<Triangle_region, Triangle_region, Triangle_region> face_edges(
    Triangle_region face) {
  switch (face) {
    case Triangle_region::LeftFace:
      return {Triangle_region::LeftEdge01, Triangle_region::LeftEdge12,
              Triangle_region::LeftEdge20};
    case Triangle_region::RightFace:
      return {Triangle_region::RightEdge01, Triangle_region::RightEdge12,
              Triangle_region::RightEdge20};
    default:
      throw std::invalid_argument("the region must be a face");
  }
}

inline std::tuple<Triangle_region, Triangle_region, Triangle_region> face_vertices(
    Triangle_region face) {
  switch (face) {
    case Triangle_region::LeftFace:
      return {Triangle_region::LeftVertex0, Triangle_region::LeftVertex1,
              Triangle_region::LeftVertex2};
    case Triangle_region::RightFace:
      return {Triangle_region::RightVertex0, Triangle_region::RightVertex1,
              Triangle_region::RightVertex2};
    default:
      throw std::invalid_argument("the region must be a face");
  }
}

inline int dimension(Triangle_region region) {
  switch (region) {
    case Triangle_region::LeftVertex0:
    case Triangle_region::LeftVertex1:
    case Triangle_region::LeftVertex2:
    case Triangle_region::RightVertex0:
    case Triangle_region::RightVertex1:
    case Triangle_region::RightVertex2:
      return 0;
    case Triangle_region::LeftEdge01:
    case Triangle_region::LeftEdge12:
    case Triangle_region::LeftEdge20:
    case Triangle_region::RightEdge01:
    case Triangle_region::RightEdge12:
    case Triangle_region::RightEdge20:
      return 1;
    case Triangle_region::LeftFace:
    case Triangle_region::RightFace:
      return 2;
    default:
      throw std::invalid_argument("the region must be a simplex");
  }
}

inline bool is_left_region(Triangle_region region) {
  return convex_hull(region, Triangle_region::LeftFace) == Triangle_region::LeftFace;
}

}  // namespace kigumi
