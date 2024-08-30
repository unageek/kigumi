#pragma once

#include <kigumi/Mesh_handles.h>
#include <kigumi/Mixed.h>
#include <kigumi/Operator.h>
#include <kigumi/Triangle_soup.h>

#include <utility>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Extract {
  using Mixed_triangle_soup = Mixed_triangle_soup<K, FaceData>;
  using Triangle_soup = Triangle_soup<K, FaceData>;

 public:
  Triangle_soup operator()(const Mixed_triangle_soup& m, Operator op, bool prefer_first) const {
    Triangle_soup soup;
    std::vector<Vertex_handle> map(m.num_vertices());

    auto u_mask = union_mask(op);
    auto i_mask = intersection_mask(op);
    auto c_mask = coplanar_mask(op, prefer_first);
    auto o_mask = opposite_mask(op, prefer_first);

    for (auto fh : m.faces()) {
      auto mask = Mask::None;
      switch (m.data(fh).tag) {
        case Face_tag::Exterior:
          mask = u_mask;
          break;
        case Face_tag::Interior:
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

      auto from_left = m.data(fh).from_left;
      auto output_id = from_left ? (mask & Mask::A) != Mask::None  //
                                 : (mask & Mask::B) != Mask::None;
      auto output_inv = from_left ? (mask & Mask::AInv) != Mask::None  //
                                  : (mask & Mask::BInv) != Mask::None;
      if (!output_id && !output_inv) {
        continue;
      }

      const auto& f = m.face(fh);
      Face new_f;
      for (std::size_t i = 0; i < 3; ++i) {
        auto vh = f.at(i);
        auto& new_vh = map.at(vh.i);
        if (new_vh == Vertex_handle{}) {
          const auto& p = m.point(vh);
          new_vh = soup.add_vertex(p);
        }
        new_f.at(i) = new_vh;
      }

      if (output_inv) {
        std::swap(new_f[1], new_f[2]);
      }

      auto new_fh = soup.add_face(new_f);
      soup.data(new_fh) = m.data(fh).data;
    }

    return soup;
  }
};

}  // namespace kigumi
