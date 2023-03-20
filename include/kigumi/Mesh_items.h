#pragma once

#include <kigumi/Mesh_handles.h>

#include <array>
#include <boost/container_hash/hash.hpp>
#include <functional>
#include <utility>

namespace kigumi {

using Edge = std::array<Vertex_handle, 2>;
using Face = std::array<Vertex_handle, 3>;

inline Edge make_edge(Vertex_handle first, Vertex_handle second) {
  if (first.i > second.i) {
    std::swap(first, second);
  }

  return {first, second};
}

}  // namespace kigumi

template <>
struct std::hash<kigumi::Edge> {
  std::size_t operator()(const kigumi::Edge& edge) const noexcept {
    std::size_t seed{};
    boost::hash_combine(seed, std::hash<std::size_t>{}(edge[0].i));
    boost::hash_combine(seed, std::hash<std::size_t>{}(edge[1].i));
    return seed;
  }
};
