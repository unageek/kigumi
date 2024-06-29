#pragma once

#include <kigumi/Null_data.h>
#include <kigumi/Triangle_mesh.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/io.h>

#include <iostream>

namespace kigumi {

enum class Face_tag { Unknown = 0, Exterior, Interior, Coplanar, Opposite };

template <>
struct Write<Face_tag> {
  static void write(std::ostream& out, const Face_tag& tt) {
    do_write<std::uint8_t>(out, static_cast<std::uint8_t>(tt));
  }
};

template <>
struct Read<Face_tag> {
  static void read(std::istream& in, Face_tag& tt) {
    std::uint8_t x{};
    do_read<std::uint8_t>(in, x);
    tt = static_cast<Face_tag>(x);
  }
};

template <class FaceData>
struct Mixed_face_data {
  bool from_left{};
  Face_tag tag{};
  FaceData data{};
};

template <class FaceData>
struct Write<Mixed_face_data<FaceData>> {
  static void write(std::ostream& out, const Mixed_face_data<FaceData>& tt) {
    do_write<bool>(out, tt.from_left);
    do_write<Face_tag>(out, tt.tag);
    do_write<FaceData>(out, tt.data);
  }
};

template <class FaceData>
struct Read<Mixed_face_data<FaceData>> {
  static void read(std::istream& in, Mixed_face_data<FaceData>& tt) {
    do_read<bool>(in, tt.from_left);
    do_read<Face_tag>(in, tt.tag);
    do_read<FaceData>(in, tt.data);
  }
};

template <class K, class FaceData = Null_data>
using Mixed_triangle_mesh = Triangle_mesh<K, Mixed_face_data<FaceData>>;

template <class K, class FaceData = Null_data>
using Mixed_triangle_soup = Triangle_soup<K, Mixed_face_data<FaceData>>;

}  // namespace kigumi
