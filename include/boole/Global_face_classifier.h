#pragma once

#include <CGAL/intersection_3.h>
#include <boole/AABB_tree/Overlap.h>
#include <boole/Mixed_mesh.h>

#include <algorithm>
#include <iterator>
#include <optional>
#include <random>
#include <vector>

namespace boole {

class Global_face_classifier {
  using FT = K::FT;
  using Leaf = Mixed_mesh::Leaf;
  using Point = K::Point_3;
  using Ray = K::Ray_3;
  using Segment = K::Segment_3;
  using Triangle = K::Triangle_3;

 public:
  explicit Global_face_classifier(Mixed_mesh& m,
                                  const std::vector<Face_handle>& representative_faces)
      : m_(m) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> dist(0, m.n_faces() - 1);

#pragma omp parallel for schedule(dynamic)
    // NOLINTNEXTLINE(modernize-loop-convert)
    for (std::size_t i = 0; i < representative_faces.size(); ++i) {
      auto fh_src = representative_faces.at(i);
      auto& f_src = m.data(fh_src);
      if (f_src.tag != Face_tag::Unknown) {
        continue;
      }

      while (true) {
        Face_handle fh_trg{dist(gen)};
        const auto& f_trg = m.data(fh_trg);
        if (f_trg.from_left == f_src.from_left) {
          continue;
        }

        auto p_src = random_point_in_triangle(m.triangle(fh_src), gen);
        auto p_trg = random_point_in_triangle(m.triangle(fh_trg), gen);

        auto result = bounded_side(p_src, p_trg, f_src.from_left);
        if (result && *result != 0) {
          f_src.tag = *result == 1 ? Face_tag::Union : Face_tag::Intersection;
          Face_tag_propagator{m, fh_src};
          break;
        }
      }
    }
  }

 private:
  struct Intersection {
    K::FT distance;
    Face_handle fh;
  };

  std::optional<CGAL::Sign> bounded_side(const Point& p_src, const Point& p_trg,
                                         bool src_from_left) const {
    if (p_src == p_trg) {
      return CGAL::ZERO;
    }

    auto ray = Ray(p_src, p_trg);
    const auto& tree = m_.aabb_tree();
    std::vector<const Leaf*> leaves;
    tree.template get_intersecting_leaves<Overlap<K>>(std::back_inserter(leaves), ray);

    std::vector<Intersection> intersections;
    for (const auto* leaf : leaves) {
      auto fh = leaf->face_handle();
      auto& f = m_.data(fh);
      if (f.from_left == src_from_left) {
        continue;
      }

      auto result = CGAL::intersection(m_.triangle(fh), ray);
      if (!result) {
        continue;
      }

      if (const auto* p = boost::get<Point>(&*result)) {
        auto d = CGAL::squared_distance(p_src, *p);
        intersections.push_back({d, fh});
      } else if (const auto* s = boost::get<Segment>(&*result)) {
        return {};
      }
    }

    if (intersections.size() >= 2) {
      std::partial_sort(intersections.begin(), intersections.begin() + 2, intersections.end(),
                        [](const auto& a, const auto& b) { return a.distance < b.distance; });

      if (intersections.at(0).distance == intersections.at(1).distance) {
        return {};
      }
    }

    auto tri = m_.triangle(intersections.at(0).fh);
    return tri.supporting_plane().oriented_side(p_src);
  }

  // CGAL::Random_points_in_triangle_3 is inexact, so use this instead.
  static Point random_point_in_triangle(const Triangle& tri, std::mt19937& gen) {
    // Use float to get less number of bits.
    std::uniform_real_distribution<float> dist(0.0, 1.0);
    const auto& p = tri[0];
    const auto& q = tri[1];
    const auto& r = tri[2];
    FT a1{dist(gen)};
    FT a2{dist(gen)};
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

  Mixed_mesh& m_;
};

}  // namespace boole
