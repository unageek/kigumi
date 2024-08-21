#pragma once

#include <kigumi/Mixed.h>

#include <unordered_set>

namespace kigumi {

template <class K, class FaceData>
class Find_border_edges {
  using Mixed_triangle_mesh = Mixed_triangle_mesh<K, FaceData>;

 public:
  std::unordered_set<Edge> operator()(const Mixed_triangle_mesh& m) const {
    std::unordered_set<Edge> left_edges;

    for (auto fh : m.faces()) {
      if (!m.data(fh).from_left) {
        continue;
      }

      const auto& f = m.face(fh);
      auto e1 = make_edge(f[0], f[1]);
      auto e2 = make_edge(f[1], f[2]);
      auto e3 = make_edge(f[2], f[0]);
      left_edges.insert(e1);
      left_edges.insert(e2);
      left_edges.insert(e3);
    }

    std::unordered_set<Edge> border_edges;

    for (auto fh : m.faces()) {
      if (m.data(fh).from_left) {
        continue;
      }

      const auto& f = m.face(fh);
      auto e1 = make_edge(f[0], f[1]);
      auto e2 = make_edge(f[1], f[2]);
      auto e3 = make_edge(f[2], f[0]);
      if (left_edges.contains(e1)) {
        border_edges.insert(e1);
      }
      if (left_edges.contains(e2)) {
        border_edges.insert(e2);
      }
      if (left_edges.contains(e3)) {
        border_edges.insert(e3);
      }
    }

    return border_edges;
  }
};

}  // namespace kigumi
