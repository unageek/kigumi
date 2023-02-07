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
  using Segment = typename K::Segment_3;
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

      if (p == p_trg) {
        return CGAL::ON_ORIENTED_BOUNDARY;
      }

      Ray ray{p, p_trg};
      tree.template get_intersecting_leaves<Overlap<K>>(std::back_inserter(leaves), ray);

      bool retry{};
      for (const auto* leaf : leaves) {
        auto fh = leaf->face_handle();
        auto tri = soup.triangle(fh);

        auto result = CGAL::intersection(tri, ray);
        if (!result) {
          continue;
        }

        if (const auto* point = boost::get<Point>(&*result)) {
          if (*point == p) {
            return CGAL::ON_ORIENTED_BOUNDARY;
          }
          auto d = CGAL::squared_distance(p, *point);
          intersections.push_back({d, fh});
        } else if (const auto* segment = boost::get<Segment>(&*result)) {
          if (segment->source() == p || segment->target() == p) {
            return CGAL::ON_ORIENTED_BOUNDARY;
          }
          retry = true;
          break;
        }
      }

      if (retry) {
        continue;
      }

      if (intersections.size() >= 2) {
        std::partial_sort(intersections.begin(), intersections.begin() + 2, intersections.end(),
                          [](const auto& a, const auto& b) { return a.distance < b.distance; });

        if (intersections.at(0).distance == intersections.at(1).distance) {
          continue;
        }
      }

      auto tri = soup.triangle(intersections.at(0).fh);
      return tri.supporting_plane().oriented_side(p);
    }
  }

 private:
  struct Intersection {
    FT distance;
    Face_handle fh;
  };
};

}  // namespace kigumi
