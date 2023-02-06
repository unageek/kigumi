#pragma once

#include <random>
#include <utility>

// CGAL::Random_points_in_triangle_3 is inexact, so use this instead.
template <class K>
typename K::Point_3 random_point_in_triangle(const typename K::Triangle_3& tri, std::mt19937& gen) {
  // Use float to get less number of bits.
  std::uniform_real_distribution<float> dist{0.0, 1.0};
  const auto& p = tri[0];
  const auto& q = tri[1];
  const auto& r = tri[2];
  typename K::FT a1{dist(gen)};
  typename K::FT a2{dist(gen)};
  if (a1 > a2) {
    std::swap(a1, a2);
  }
  auto b1 = a1;
  auto b2 = a2 - a1;
  auto b3 = 1.0 - a2;
  return {
      b1 * p.x() + b2 * q.x() + b3 * r.x(),
      b1 * p.y() + b2 * q.y() + b3 * r.y(),
      b1 * p.z() + b2 * q.z() + b3 * r.z(),
  };
}
