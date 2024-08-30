#pragma once

#include <kigumi/Face_tag.h>
#include <kigumi/Null_data.h>
#include <kigumi/Triangle_mesh.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/io.h>

#include <iostream>

namespace kigumi {

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
