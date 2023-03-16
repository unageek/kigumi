#pragma once

#include <kigumi/AABB_tree/Overlap.h>
#include <kigumi/Face_tag_propagator.h>
#include <kigumi/Mixed.h>
#include <kigumi/Side_of_triangle_soup.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/random_point_in_triangle.h>

#include <queue>
#include <random>
#include <unordered_set>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Global_face_classifier {
 public:
  explicit Global_face_classifier(Mixed_triangle_mesh<K, FaceData>& m,
                                  const std::unordered_set<Edge>& border,
                                  const Triangle_soup<K, FaceData>& left,
                                  const Triangle_soup<K, FaceData>& right)
      : m_{m}, border_{border} {
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<std::size_t> dist{0, m.num_faces() - 1};

    auto representative_faces = find_unclassified_connected_components();

#pragma omp parallel for schedule(dynamic)
    for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(representative_faces.size()); ++i) {
      auto fh_src = representative_faces.at(i);
      auto& f_src = m.data(fh_src);
      auto tri_src = m.triangle(fh_src);
      const auto& soup_trg = f_src.from_left ? right : left;
      while (true) {
        auto p_src = random_point_in_triangle<K>(tri_src, gen);
        auto side = Side_of_triangle_soup<K, FaceData>{}(soup_trg, p_src);
        if (side != CGAL::ON_ORIENTED_BOUNDARY) {
          f_src.tag = side == CGAL::ON_POSITIVE_SIDE ? Face_tag::Union : Face_tag::Intersection;
          Face_tag_propagator{m, border, fh_src};
          break;
        }
      }
    }
  }

 private:
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
