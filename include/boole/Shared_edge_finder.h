#pragma once

#include <boole/Mixed_mesh.h>

#include <boost/container_hash/hash.hpp>
#include <functional>
#include <unordered_set>
#include <utility>
#include <vector>

namespace boole {

class Shared_edge_finder {
 public:
  explicit Shared_edge_finder(const Mixed_mesh& m) {
    std::unordered_set<Edge> left_edges;
    std::unordered_set<Edge> shared_edges;

    for (auto fh : m.faces()) {
      if (!m.data(fh).from_left) {
        continue;
      }

      const auto& f = m.face(fh);
      auto i = f[0];
      auto j = f[1];
      auto k = f[2];
      left_edges.insert(make_edge(i, j));
      left_edges.insert(make_edge(j, k));
      left_edges.insert(make_edge(k, i));
    }

    for (auto fh : m.faces()) {
      if (m.data(fh).from_left) {
        continue;
      }

      const auto f = m.face(fh);
      auto i = f[0];
      auto j = f[1];
      auto k = f[2];
      if (left_edges.contains(make_edge(i, j))) {
        shared_edges.insert(make_edge(i, j));
      }
      if (left_edges.contains(make_edge(j, k))) {
        shared_edges.insert(make_edge(j, k));
      }
      if (left_edges.contains(make_edge(k, i))) {
        shared_edges.insert(make_edge(k, i));
      }
    }

    shared_edges_ = std::vector<Edge>(shared_edges.begin(), shared_edges.end());
  }

  const std::vector<Edge>& shared_edges() const { return shared_edges_; }

 private:
  static Edge make_edge(Vertex_handle first, Vertex_handle second) {
    if (first.i > second.i) {
      std::swap(first, second);
    }

    return {first, second};
  }

  std::vector<Edge> shared_edges_;
};

}  // namespace boole
