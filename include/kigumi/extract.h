#pragma once

#include <kigumi/Mixed_mesh.h>
#include <kigumi/Operator.h>
#include <kigumi/Polygon_soup.h>

#include <array>
#include <unordered_map>
#include <utility>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
Polygon_soup<K, FaceData> extract(const Mixed_polygon_soup<K, FaceData>& m, Operator op,
                                  bool extract_first, bool extract_second, bool prefer_first) {
  Polygon_soup<K, FaceData> soup;
  std::unordered_map<Vertex_handle, Vertex_handle> map;

  auto u_mask = union_mask(op);
  auto i_mask = intersection_mask(op);
  auto c_mask = coplanar_mask(op, prefer_first);
  auto o_mask = opposite_mask(op, prefer_first);

  for (auto fh : m.faces()) {
    auto from_left = m.data(fh).from_left;
    if ((from_left && !extract_first) || (!from_left && !extract_second)) {
      continue;
    }

    auto mask = Mask::None;
    switch (m.data(fh).tag) {
      case Face_tag::Union:
        mask = u_mask;
        break;
      case Face_tag::Intersection:
        mask = i_mask;
        break;
      case Face_tag::Coplanar:
        mask = c_mask;
        break;
      case Face_tag::Opposite:
        mask = o_mask;
        break;
      case Face_tag::Unknown:
        break;
    }

    auto output_id = from_left ? (mask & Mask::A) != Mask::None  //
                               : (mask & Mask::B) != Mask::None;
    auto output_inv = from_left ? (mask & Mask::AInv) != Mask::None  //
                                : (mask & Mask::BInv) != Mask::None;
    if (!output_id && !output_inv) {
      continue;
    }

    const auto& f = m.face(fh);
    for (auto vh : f) {
      const auto& p = m.point(vh);
      if (!map.contains(vh)) {
        map.emplace(vh, soup.add_vertex(p));
      }
    }

    auto new_fh = output_inv ? soup.add_face({map.at(f[0]), map.at(f[2]), map.at(f[1])})
                             : soup.add_face({map.at(f[0]), map.at(f[1]), map.at(f[2])});
    soup.data(new_fh) = m.data(fh).data;
  }

  return soup;
}

}  // namespace kigumi
