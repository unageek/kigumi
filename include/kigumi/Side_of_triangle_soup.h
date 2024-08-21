#pragma once

#include <CGAL/Intersections_3/Ray_3_Triangle_3.h>
#include <kigumi/AABB_tree/Overlap.h>
#include <kigumi/Triangle_soup.h>

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Side_of_triangle_soup {
  using FT = typename K::FT;
  using Leaf = typename Triangle_soup<K, FaceData>::Leaf;
  using Point = typename K::Point_3;
  using Ray = typename K::Ray_3;
  using Segment = typename K::Segment_3;
  using Triangle_soup = Triangle_soup<K, FaceData>;

 public:
  CGAL::Oriented_side operator()(const Triangle_soup& soup, const Point& p) const {
    if (soup.num_faces() == 0) {
      throw std::runtime_error("triangle soup must not be empty");
    }

    const auto& tree = soup.aabb_tree();

    for (auto fh_trg : soup.faces()) {
      leaves_.clear();
      intersections_.clear();

      auto tri_trg = soup.triangle(fh_trg);
      auto p_trg = CGAL::centroid(tri_trg);

      if (p == p_trg) {
        return CGAL::ON_ORIENTED_BOUNDARY;
      }

      Ray ray{p, p_trg};
      tree.template get_intersecting_leaves<Overlap<K>>(std::back_inserter(leaves_), ray);

      for (const auto* leaf : leaves_) {
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
          intersections_.emplace_back(std::move(d), fh);
        } else if (const auto* segment = boost::get<Segment>(&*result)) {
          if (segment->source() == p || segment->target() == p) {
            return CGAL::ON_ORIENTED_BOUNDARY;
          }
          // Ignore.
        }
      }

      if (intersections_.empty()) {
        // The point is outside the convex boundary of the mesh, etc.
        throw std::runtime_error("cannot determine the side of the point");
      }

      if (intersections_.size() >= 2) {
        std::partial_sort(intersections_.begin(), intersections_.begin() + 2, intersections_.end(),
                          [](const auto& a, const auto& b) { return a.distance < b.distance; });

        if (intersections_.at(0).distance == intersections_.at(1).distance) {
          // The ray touches or passes through an edge or a vertex shared by two or more triangles.
          continue;
        }
      }

      auto tri = soup.triangle(intersections_.at(0).fh);
      return tri.supporting_plane().oriented_side(p);
    }

    // Should not happen.
    throw std::runtime_error("cannot determine the side of the point");
  }

 private:
  struct Intersection {
    FT distance;
    Face_handle fh;
  };

  mutable std::vector<const Leaf*> leaves_;
  mutable std::vector<Intersection> intersections_;
};

}  // namespace kigumi
