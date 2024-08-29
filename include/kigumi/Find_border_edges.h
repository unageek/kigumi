#pragma once

#include <kigumi/Mesh_entities.h>
#include <kigumi/Mixed.h>
#include <kigumi/Triangle_soup.h>

#include <unordered_set>

namespace kigumi {

template <class K, class FaceData>
class Find_border_edges {
  using Mixed_triangle_mesh = Mixed_triangle_mesh<K, FaceData>;
  using Triangle_soup = Triangle_soup<K, FaceData>;

 public:
  std::unordered_set<Edge> operator()(const Mixed_triangle_mesh& m, const Triangle_soup& left,
                                      const Triangle_soup& right) const {
    auto left_is_a = left.num_faces() < right.num_faces();
    std::unordered_set<Edge> a_edges;

    for (auto fh : m.faces()) {
      if (m.data(fh).from_left != left_is_a) {
        continue;
      }

      const auto& f = m.face(fh);
      auto e1 = make_edge(f[0], f[1]);
      auto e2 = make_edge(f[1], f[2]);
      auto e3 = make_edge(f[2], f[0]);
      a_edges.insert(e1);
      a_edges.insert(e2);
      a_edges.insert(e3);
    }

    std::unordered_set<Edge> border_edges;

    for (auto fh : m.faces()) {
      if (m.data(fh).from_left == left_is_a) {
        continue;
      }

      const auto& f = m.face(fh);
      auto e1 = make_edge(f[0], f[1]);
      auto e2 = make_edge(f[1], f[2]);
      auto e3 = make_edge(f[2], f[0]);
      if (a_edges.contains(e1)) {
        border_edges.insert(e1);
      }
      if (a_edges.contains(e2)) {
        border_edges.insert(e2);
      }
      if (a_edges.contains(e3)) {
        border_edges.insert(e3);
      }
    }

    return border_edges;
  }
};

}  // namespace kigumi
