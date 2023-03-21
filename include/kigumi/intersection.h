#pragma once

#include <CGAL/Intersections_3/Line_3_Triangle_3.h>
#include <CGAL/Intersections_3/Point_3_Triangle_3.h>
#include <CGAL/Intersections_3/Triangle_3_Triangle_3.h>

#include <set>

namespace kigumi {

template <class K>
void insert_intersection(const typename K::Line_3& l, const typename K::Triangle_3& t,
                         std::set<typename K::Point_3>& points) {
  using Point = typename K::Point_3;

  auto inter = CGAL::intersection(l, t);
  if (!inter) {
    return;
  }

  const auto* point = boost::get<Point>(&*inter);
  points.insert(*point);
}

template <class K>
void insert_intersection(const typename K::Point_3& p, const typename K::Triangle_3& t,
                         std::set<typename K::Point_3>& points) {
  using Point = typename K::Point_3;

  auto inter = CGAL::intersection(p, t);
  if (!inter) {
    return;
  }

  const auto* point = boost::get<Point>(&*inter);
  points.insert(*point);
}

inline constexpr int signs(int s1, int s2, int s3) {
  // 9 * s1 + 3 * s2 + s3 is also fine.
  return 16 * s1 + 4 * s2 + s3;
}

template <class Triangle>
auto intersection_fast(const Triangle& t1, const Triangle& t2)
    -> decltype(CGAL::intersection(t1, t2)) {
  using K = typename CGAL::Kernel_traits<Triangle>::Kernel;
  using Line = typename K::Line_3;
  using Point = typename K::Point_3;
  using Segment = typename K::Segment_3;

  auto plane1 = t1.supporting_plane();
  auto plane2 = t2.supporting_plane();

  auto signs1 = signs(static_cast<int>(plane2.oriented_side(t1.vertex(0))),
                      static_cast<int>(plane2.oriented_side(t1.vertex(1))),
                      static_cast<int>(plane2.oriented_side(t1.vertex(2))));

  auto signs2 = signs(static_cast<int>(plane1.oriented_side(t2.vertex(0))),
                      static_cast<int>(plane1.oriented_side(t2.vertex(1))),
                      static_cast<int>(plane1.oriented_side(t2.vertex(2))));

  for (auto ss : {signs1, signs2}) {
    switch (ss) {
      // No intersections.
      case signs(-1, -1, -1):
      case signs(1, 1, 1):
        return {};

      // Coplanar.
      case signs(0, 0, 0):
        return CGAL::Intersections::internal::intersection_coplanar_triangles(t1, t2, K{});
    }
  }

  std::set<Point> points;

  for (auto i = 0; i < 2; ++i) {
    const auto& v = i == 0 ? t1 : t2;
    const auto& t = i == 0 ? t2 : t1;
    std::array<Line, 3> e{Line{v[1], v[2]}, Line{v[2], v[0]}, Line{v[0], v[1]}};

    auto ss = i == 0 ? signs1 : signs2;
    switch (ss) {
      // One vertex and the other two vertices are on opposite sides of the plane.
      case signs(-1, 1, 1):
      case signs(1, -1, -1):
        insert_intersection<K>(e[1], t, points);
        insert_intersection<K>(e[2], t, points);
        break;
      case signs(-1, 1, -1):
      case signs(1, -1, 1):
        insert_intersection<K>(e[2], t, points);
        insert_intersection<K>(e[0], t, points);
        break;
      case signs(-1, -1, 1):
      case signs(1, 1, -1):
        insert_intersection<K>(e[0], t, points);
        insert_intersection<K>(e[1], t, points);
        break;

      // One vertex is on the plane, the other two vertices are on opposite sides of the plane.
      case signs(0, -1, 1):
      case signs(0, 1, -1):
        insert_intersection<K>(e[0], t, points);
        insert_intersection<K>(v[0], t, points);
        break;
      case signs(-1, 0, 1):
      case signs(1, 0, -1):
        insert_intersection<K>(e[1], t, points);
        insert_intersection<K>(v[1], t, points);
        break;
      case signs(-1, 1, 0):
      case signs(1, -1, 0):
        insert_intersection<K>(e[2], t, points);
        insert_intersection<K>(v[2], t, points);
        break;

      // Two vertices are on the plane.
      case signs(-1, 0, 0):
      case signs(1, 0, 0):
        insert_intersection<K>(v[1], t, points);
        insert_intersection<K>(v[2], t, points);
        break;
      case signs(0, -1, 0):
      case signs(0, 1, 0):
        insert_intersection<K>(v[2], t, points);
        insert_intersection<K>(v[0], t, points);
        break;
      case signs(0, 0, -1):
      case signs(0, 0, 1):
        insert_intersection<K>(v[0], t, points);
        insert_intersection<K>(v[1], t, points);
        break;

      // One vertex is on the plane, the other two vertices are on the same side of the plane.
      case signs(0, -1, -1):
      case signs(0, 1, 1):
        insert_intersection<K>(v[0], t, points);
        break;
      case signs(-1, 0, -1):
      case signs(1, 0, 1):
        insert_intersection<K>(v[1], t, points);
        break;
      case signs(-1, -1, 0):
      case signs(1, 1, 0):
        insert_intersection<K>(v[2], t, points);
        break;
    }
  }

  if (points.size() == 0) {
    return {};
  }

  auto it = points.begin();
  if (points.size() == 1) {
    const auto& p = *it++;
    return {{p}};
  }

  const auto& p = *it++;
  const auto& q = *it++;
  return {{Segment{p, q}}};
}

}  // namespace kigumi
