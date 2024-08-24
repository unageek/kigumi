#pragma once

#include <kigumi/Null_data.h>
#include <kigumi/Triangle_mesh.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/io.h>

#include <cstdint>
#include <iostream>

namespace kigumi {

enum class Face_tag : std::uint8_t { Unknown = 0, Exterior, Interior, Coplanar, Opposite };

template <>
struct Write<Face_tag> {
  void operator()(std::ostream& out, const Face_tag& t) const {
    kigumi_write<std::uint8_t>(out, static_cast<std::uint8_t>(t));
  }
};

template <>
struct Read<Face_tag> {
  void operator()(std::istream& in, Face_tag& t) const {
    std::uint8_t x{};
    kigumi_read<std::uint8_t>(in, x);
    t = static_cast<Face_tag>(x);
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
  void operator()(std::ostream& out, const Mixed_face_data<FaceData>& t) const {
    kigumi_write<bool>(out, t.from_left);
    kigumi_write<Face_tag>(out, t.tag);
    kigumi_write<FaceData>(out, t.data);
  }
};

template <class FaceData>
struct Read<Mixed_face_data<FaceData>> {
  void operator()(std::istream& in, Mixed_face_data<FaceData>& t) const {
    kigumi_read<bool>(in, t.from_left);
    kigumi_read<Face_tag>(in, t.tag);
    kigumi_read<FaceData>(in, t.data);
  }
};

template <class K, class FaceData = Null_data>
using Mixed_triangle_mesh = Triangle_mesh<K, Mixed_face_data<FaceData>>;

template <class K, class FaceData = Null_data>
using Mixed_triangle_soup = Triangle_soup<K, Mixed_face_data<FaceData>>;

}  // namespace kigumi
