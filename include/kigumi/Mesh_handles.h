#pragma once

#include <kigumi/io.h>

#include <functional>
#include <limits>

namespace kigumi {

struct Face_handle {
  std::size_t i{std::numeric_limits<std::size_t>::max()};
};

inline bool operator<(Face_handle a, Face_handle b) { return a.i < b.i; }
inline bool operator==(Face_handle a, Face_handle b) { return a.i == b.i; }
inline bool operator!=(Face_handle a, Face_handle b) { return a.i != b.i; }

struct Vertex_handle {
  std::size_t i{std::numeric_limits<std::size_t>::max()};
};

inline bool operator<(Vertex_handle a, Vertex_handle b) { return a.i < b.i; }
inline bool operator==(Vertex_handle a, Vertex_handle b) { return a.i == b.i; }
inline bool operator!=(Vertex_handle a, Vertex_handle b) { return a.i != b.i; }

template <>
struct Write<Vertex_handle> {
  static void write(std::ostream& out, const Vertex_handle& tt) {
    do_write<std::int32_t>(out, tt.i);
  }
};

template <>
struct Read<Vertex_handle> {
  static void read(std::istream& in, Vertex_handle& tt) { do_read<std::int32_t>(in, tt.i); }
};

}  // namespace kigumi

template <>
struct std::hash<kigumi::Face_handle> {
  std::size_t operator()(kigumi::Face_handle fh) const noexcept { return fh.i; }
};

template <>
struct std::hash<kigumi::Vertex_handle> {
  std::size_t operator()(kigumi::Vertex_handle vh) const noexcept { return vh.i; }
};
