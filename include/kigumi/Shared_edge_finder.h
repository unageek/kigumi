#pragma once

#include <kigumi/Mixed.h>

#include <unordered_set>

namespace kigumi {

template <class K, class FaceData>
class Shared_edge_finder {
 public:
  explicit Shared_edge_finder(const Mixed_triangle_mesh<K, FaceData>& m) {
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

    for (auto fh : m.faces()) {
      if (m.data(fh).from_left) {
        continue;
      }

      const auto& f = m.face(fh);
      auto e1 = make_edge(f[0], f[1]);
      auto e2 = make_edge(f[1], f[2]);
      auto e3 = make_edge(f[2], f[0]);
      if (left_edges.contains(e1)) {
        shared_edges_.insert(e1);
      }
      if (left_edges.contains(e2)) {
        shared_edges_.insert(e2);
      }
      if (left_edges.contains(e3)) {
        shared_edges_.insert(e3);
      }
    }
  }

  const std::unordered_set<Edge>& shared_edges() const { return shared_edges_; }

 private:
  std::unordered_set<Edge> shared_edges_;
};

}  // namespace kigumi
