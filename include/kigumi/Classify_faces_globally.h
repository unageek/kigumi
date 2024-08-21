#pragma once

#include <kigumi/Mesh_handles.h>
#include <kigumi/Mixed.h>
#include <kigumi/Propagate_face_tags.h>
#include <kigumi/Side_of_triangle_soup.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Warnings.h>
#include <kigumi/parallel_do.h>

#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Classify_faces_globally {
  using Mixed_triangle_mesh = Mixed_triangle_mesh<K, FaceData>;
  using Propagate_face_tags = Propagate_face_tags<K, FaceData>;
  using Side_of_triangle_soup = Side_of_triangle_soup<K, FaceData>;
  using Triangle_soup = Triangle_soup<K, FaceData>;

 public:
  Warnings operator()(Mixed_triangle_mesh& m, const std::unordered_set<Edge>& border_edges,
                      const Triangle_soup& left, const Triangle_soup& right) const {
    auto representative_faces = find_unclassified_connected_components(m, border_edges);
    Warnings warnings{};

    parallel_do(
        representative_faces.begin(), representative_faces.end(), Warnings{},
        [&](auto fh_src, auto& local_warnings) {
          thread_local Propagate_face_tags propagate_face_tags;
          thread_local Side_of_triangle_soup side_of_triangle_soup;

          auto& f_src = m.data(fh_src);
          auto tri_src = m.triangle(fh_src);
          const auto& soup_trg = f_src.from_left ? right : left;
          auto p_src = CGAL::centroid(tri_src);
          auto side = side_of_triangle_soup(soup_trg, p_src);
          if (side == CGAL::ON_ORIENTED_BOUNDARY) {
            throw std::runtime_error(
                "local classification must be performed before global classification");
          }
          f_src.tag = side == CGAL::ON_POSITIVE_SIDE ? Face_tag::Exterior : Face_tag::Interior;
          local_warnings |= propagate_face_tags(m, border_edges, fh_src);
        },
        [&](const auto& local_warnings) { warnings |= local_warnings; });

    return warnings;
  }

 private:
  static std::vector<Face_handle> find_unclassified_connected_components(
      const Mixed_triangle_mesh& m, const std::unordered_set<Edge>& border_edges) {
    std::vector<Face_handle> representative_faces;
    std::vector<bool> visited(m.num_faces(), false);
    std::queue<Face_handle> queue;

    auto begin = m.faces_begin();
    auto end = m.faces_end();

    while (true) {
      for (auto it = begin; it != end; ++it) {
        auto fh = *it;
        if (visited.at(fh.i)) {
          continue;
        }

        visited.at(fh.i) = true;
        if (m.data(fh).tag == Face_tag::Unknown) {
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

        for (auto adj_fh : m.faces_around_face(fh, border_edges)) {
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
};

}  // namespace kigumi
