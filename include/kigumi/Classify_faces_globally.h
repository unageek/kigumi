#pragma once

#include <CGAL/enum.h>
#include <kigumi/Face_tag.h>
#include <kigumi/Mesh_entities.h>
#include <kigumi/Mesh_indices.h>
#include <kigumi/Mixed.h>
#include <kigumi/Propagate_face_tags.h>
#include <kigumi/Side_of_triangle_soup.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Warnings.h>
#include <kigumi/mesh_utility.h>
#include <kigumi/parallel_do.h>

#include <queue>
#include <stdexcept>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Classify_faces_globally {
  using Mixed_triangle_mesh = Mixed_triangle_mesh<K, FaceData>;
  using Propagate_face_tags = Propagate_face_tags<K, FaceData>;
  using Side_of_triangle_soup = Side_of_triangle_soup<K, FaceData>;
  using Triangle_soup = Triangle_soup<K, FaceData>;

 public:
  Warnings operator()(Mixed_triangle_mesh& m, const Edge_set& border_edges,
                      const Triangle_soup& left, const Triangle_soup& right) const {
    auto representative_faces = find_unclassified_connected_components(m, border_edges);
    Warnings warnings{};

    parallel_do(
        representative_faces.begin(), representative_faces.end(), Warnings{},
        [&](auto fi_src, auto& local_warnings) {
          thread_local Propagate_face_tags propagate_face_tags;
          thread_local Side_of_triangle_soup side_of_triangle_soup;

          auto& f_src = m.data(fi_src);
          const auto& soup_trg = f_src.from_left ? right : left;
          auto p_src = internal::face_centroid(m, fi_src);
          auto side = side_of_triangle_soup(soup_trg, p_src);
          if (side == CGAL::ON_ORIENTED_BOUNDARY) {
            throw std::runtime_error(
                "local classification must be performed before global classification");
          }
          f_src.tag = side == CGAL::ON_POSITIVE_SIDE ? Face_tag::EXTERIOR : Face_tag::INTERIOR;
          local_warnings |= propagate_face_tags(m, border_edges, fi_src);
        },
        [&](const auto& local_warnings) { warnings |= local_warnings; });

    return warnings;
  }

 private:
  static std::vector<Face_index> find_unclassified_connected_components(
      const Mixed_triangle_mesh& m, const Edge_set& border_edges) {
    std::vector<Face_index> representative_faces;
    std::vector<bool> visited(m.num_faces(), false);
    std::queue<Face_index> queue;

    auto begin = m.faces_begin();
    auto end = m.faces_end();

    while (true) {
      for (auto it = begin; it != end; ++it) {
        auto fi = *it;
        if (visited.at(fi.idx())) {
          continue;
        }

        visited.at(fi.idx()) = true;
        if (m.data(fi).tag == Face_tag::UNKNOWN) {
          queue.push(fi);
          representative_faces.push_back(fi);
          begin = ++it;
          break;
        }
      }

      if (queue.empty()) {
        break;
      }

      while (!queue.empty()) {
        auto fi = queue.front();
        queue.pop();

        for (auto adj_fi : m.faces_around_face(fi, border_edges)) {
          if (visited.at(adj_fi.idx())) {
            continue;
          }

          visited.at(adj_fi.idx()) = true;
          queue.push(adj_fi);
        }
      }
    }

    return representative_faces;
  }
};

}  // namespace kigumi
