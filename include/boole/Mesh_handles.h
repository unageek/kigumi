#pragma once

#include <functional>
#include <limits>

namespace boole {

struct Face_handle {
  std::size_t i = std::numeric_limits<std::size_t>::max();
};

inline bool operator<(Face_handle a, Face_handle b) { return a.i < b.i; }
inline bool operator==(Face_handle a, Face_handle b) { return a.i == b.i; }
inline bool operator!=(Face_handle a, Face_handle b) { return a.i != b.i; }

struct Vertex_handle {
  std::size_t i = std::numeric_limits<std::size_t>::max();
};

inline bool operator<(Vertex_handle a, Vertex_handle b) { return a.i < b.i; }
inline bool operator==(Vertex_handle a, Vertex_handle b) { return a.i == b.i; }
inline bool operator!=(Vertex_handle a, Vertex_handle b) { return a.i != b.i; }

}  // namespace boole

template <>
struct std::hash<boole::Vertex_handle> {
  std::size_t operator()(boole::Vertex_handle vh) const { return vh.i; }
};
