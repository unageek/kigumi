#pragma once

#include <CGAL/Intersections_3/Ray_3_Triangle_3.h>
#include <kigumi/AABB_tree/Overlap.h>
#include <kigumi/Mixed.h>
#include <kigumi/random_point_in_triangle.h>

#include <algorithm>
#include <iterator>
#include <optional>
#include <queue>
#include <random>
#include <unordered_set>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Global_face_classifier {
  using FT = typename K::FT;
  using Leaf = typename Mixed_triangle_mesh<K, FaceData>::Leaf;
  using Point = typename K::Point_3;
  using Ray = typename K::Ray_3;
  using Segment = typename K::Segment_3;
  using Triangle = typename K::Triangle_3;

 public:
  explicit Global_face_classifier(Mixed_triangle_mesh<K, FaceData>& m,
                                  const std::unordered_set<Edge>& border)
      : m_{m}, border_{border} {
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<std::size_t> dist{0, m.num_faces() - 1};

    auto representative_faces = find_unclassified_connected_components();

#pragma omp parallel for schedule(dynamic)
    for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(representative_faces.size()); ++i) {
      auto fh_src = representative_faces.at(i);
      auto& f_src = m.data(fh_src);

      while (true) {
        Face_handle fh_trg{dist(gen)};
        const auto& f_trg = m.data(fh_trg);
        if (f_trg.from_left == f_src.from_left) {
          continue;
        }

        auto p_src = random_point_in_triangle<K>(m.triangle(fh_src), gen);
        auto p_trg = random_point_in_triangle<K>(m.triangle(fh_trg), gen);

        auto result = oriented_side(p_src, p_trg, f_src.from_left);
        if (result && *result != CGAL::ON_ORIENTED_BOUNDARY) {
          f_src.tag = *result == CGAL::ON_POSITIVE_SIDE ? Face_tag::Union : Face_tag::Intersection;
          Face_tag_propagator{m, border, fh_src};
          break;
        }
      }
    }
  }

 private:
  struct Intersection {
    FT distance;
    Face_handle fh;
  };

  std::optional<CGAL::Oriented_side> oriented_side(const Point& p_src, const Point& p_trg,
                                                   bool src_from_left) const {
    if (p_src == p_trg) {
      return CGAL::ON_ORIENTED_BOUNDARY;
    }

    Ray ray{p_src, p_trg};
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
        if (*p == p_src) {
          return CGAL::ON_ORIENTED_BOUNDARY;
        }
        auto d = CGAL::squared_distance(p_src, *p);
        intersections.push_back({d, fh});
      } else if (const auto* s = boost::get<Segment>(&*result)) {
        if (s->source() == p_src && s->target() == p_src) {
          return CGAL::ON_ORIENTED_BOUNDARY;
        }
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

  std::vector<Face_handle> find_unclassified_connected_components() {
    std::vector<Face_handle> representative_faces;
    std::vector<bool> visited(m_.num_faces(), false);
    std::queue<Face_handle> queue;

    auto begin = m_.faces_begin();
    auto end = m_.faces_end();

    while (true) {
      for (auto it = begin; it != end; ++it) {
        auto fh = *it;
        if (visited.at(fh.i)) {
          continue;
        }

        visited.at(fh.i) = true;
        if (m_.data(fh).tag == Face_tag::Unknown) {
          queue.push(fh);
          representative_faces.push_back(fh);
          begin = ++it;
          break;
        }
      }

      if (queue.empty()) {
        break;
      }

      while (!queue.empty()) {
        auto fh = queue.front();
        queue.pop();

        for (auto adj_fh : m_.faces_around_face(fh, border_)) {
          if (visited.at(adj_fh.i)) {
            continue;
          }

          visited.at(adj_fh.i) = true;
          queue.push(adj_fh);
        }
      }
    }

    return representative_faces;
  }

  Mixed_triangle_mesh<K, FaceData>& m_;
  const std::unordered_set<Edge>& border_;
};

}  // namespace kigumi
