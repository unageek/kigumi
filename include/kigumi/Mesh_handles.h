#pragma once

#include <kigumi/io.h>

#include <cstdint>
#include <functional>
#include <limits>

namespace kigumi {

struct Face_handle {
  Face_handle() : i{std::numeric_limits<std::size_t>::max()} {}

  Face_handle(std::size_t i) : i{i} {}

  std::size_t i;
};

inline bool operator<(Face_handle a, Face_handle b) { return a.i < b.i; }
inline bool operator==(Face_handle a, Face_handle b) { return a.i == b.i; }
inline bool operator!=(Face_handle a, Face_handle b) { return a.i != b.i; }

struct Vertex_handle {
  Vertex_handle() : i{std::numeric_limits<std::size_t>::max()} {}

  Vertex_handle(std::size_t i) : i{i} {}

  std::size_t i;
};

inline bool operator<(Vertex_handle a, Vertex_handle b) { return a.i < b.i; }
inline bool operator==(Vertex_handle a, Vertex_handle b) { return a.i == b.i; }
inline bool operator!=(Vertex_handle a, Vertex_handle b) { return a.i != b.i; }

template <>
struct Write<Vertex_handle> {
  void operator()(std::ostream& out, const Vertex_handle& t) const {
    kigumi_write<std::int32_t>(out, t.i);
  }
};

template <>
struct Read<Vertex_handle> {
  void operator()(std::istream& in, Vertex_handle& t) const { kigumi_read<std::int32_t>(in, t.i); }
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
