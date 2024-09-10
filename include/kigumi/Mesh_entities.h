#pragma once

#include <kigumi/Mesh_indices.h>

#include <array>
#include <boost/container_hash/hash.hpp>
#include <functional>
#include <utility>

namespace kigumi {

using Edge = std::array<Vertex_index, 2>;
using Face = std::array<Vertex_index, 3>;

inline Edge make_edge(Vertex_index first, Vertex_index second) {
  if (first > second) {
    std::swap(first, second);
  }

  return {first, second};
}

}  // namespace kigumi

template <>
struct std::hash<kigumi::Edge> {
  std::size_t operator()(const kigumi::Edge& edge) const noexcept {
    std::size_t seed{};
    boost::hash_combine(seed, edge[0].idx());
    boost::hash_combine(seed, edge[1].idx());
    return seed;
  }
};
