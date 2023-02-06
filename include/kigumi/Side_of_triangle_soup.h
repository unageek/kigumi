#pragma once

#include <CGAL/Intersections_3/Ray_3_Triangle_3.h>
#include <kigumi/AABB_tree/Overlap.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/random_point_in_triangle.h>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <random>
#include <stdexcept>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Side_of_triangle_soup {
  using FT = typename K::FT;
  using Leaf = typename Triangle_soup<K, FaceData>::Leaf;
  using Point = typename K::Point_3;
  using Ray = typename K::Ray_3;

 public:
  CGAL::Oriented_side operator()(const Triangle_soup<K, FaceData>& soup,
                                 const typename K::Point_3& p) {
    if (soup.num_faces() == 0) {
      throw std::runtime_error("triangle soup must not be empty.");
    }

    const auto& tree = soup.aabb_tree();
    std::vector<const Leaf*> leaves;
    std::vector<Intersection> intersections;

    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<std::size_t> dist{0, soup.num_faces() - 1};

    while (true) {
      leaves.clear();
      intersections.clear();

      Face_handle fh_trg{dist(gen)};
      auto p_trg = random_point_in_triangle<K>(soup.triangle(fh_trg), gen);

      Ray ray{p, p_trg};
      tree.get_intersecting_leaves<Overlap<K>>(std::back_inserter(leaves), ray);

      bool retry{};
      for (const auto* leaf : leaves) {
        auto fh = leaf->face_handle();
        auto tri = soup.triangle(fh);
        CGAL::Intersections::internal::r3t3_do_intersect_endpoint_position_visitor visitor;
        auto res = CGAL::Intersections::internal::do_intersect(tri, ray, K(), visitor);
        if (!res.first) {
          continue;
        }

        switch (res.second) {
          case CGAL::Intersections::internal::R3T3_intersection::ENDPOINT_IN_TRIANGLE:
            return CGAL::ON_ORIENTED_BOUNDARY;

          case CGAL::Intersections::internal::R3T3_intersection::COPLANAR_RAY:
          case CGAL::Intersections::internal::R3T3_intersection::CROSS_SEGMENT:
          case CGAL::Intersections::internal::R3T3_intersection::CROSS_VERTEX:
            retry = true;
            break;

          case CGAL::Intersections::internal::R3T3_intersection::CROSS_FACET: {
            auto result = CGAL::intersection(tri, ray);
            if (result) {
              if (const auto* point = boost::get<Point>(&*result)) {
                auto d = CGAL::squared_distance(p, *point);
                intersections.push_back({d, fh});
              }
            }
            break;
          }

          default:
            assert(false);
            return CGAL::ON_ORIENTED_BOUNDARY;
        }
      }

      if (retry) {
        continue;
      }

      auto it = std::min_element(
          intersections.begin(), intersections.end(),
          [](const auto& a, const auto& b) { return a.squared_distance < b.squared_distance; });

      auto tri = soup.triangle(it->fh);
      return tri.supporting_plane().oriented_side(p);
    }
  }

 private:
  struct Intersection {
    FT squared_distance;
    Face_handle fh;
  };
};

}  // namespace kigumi
