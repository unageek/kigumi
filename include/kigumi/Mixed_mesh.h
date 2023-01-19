#pragma once

#include <kigumi/Mesh.h>
#include <kigumi/Polygon_soup.h>
#include <kigumi/io.h>

#include <iostream>

namespace kigumi {

enum class Face_tag { Unknown = 0, Union, Intersection, Coplanar, Opposite };

template <>
struct Write<Face_tag> {
  static void write(std::ostream& out, const Face_tag& tt) {
    do_write(out, static_cast<std::uint8_t>(tt));
  }
};

template <>
struct Read<Face_tag> {
  static void read(std::istream& in, Face_tag& tt) {
    std::uint8_t x{};
    do_read(in, x);
    tt = static_cast<Face_tag>(x);
  }
};

struct Face_data {
  bool from_left = false;
  Face_tag tag = Face_tag::Unknown;
};

template <>
struct Write<Face_data> {
  static void write(std::ostream& out, const Face_data& tt) {
    do_write(out, tt.from_left);
    do_write(out, tt.tag);
  }
};

template <>
struct Read<Face_data> {
  static void read(std::istream& in, Face_data& tt) {
    do_read(in, tt.from_left);
    do_read(in, tt.tag);
  }
};

template <class K>
using Mixed_mesh = Mesh<K, Face_data>;

template <class K>
using Mixed_polygon_soup = Polygon_soup<K, Face_data>;

}  // namespace kigumi
