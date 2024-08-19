#pragma once

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Kernel/global_functions.h>
#include <CGAL/Projection_traits_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>

#include <array>

namespace kigumi {

template <class K>
class Triangulator {
  using Point = typename K::Point_3;
  using Triangle = typename K::Triangle_3;
  using CDT_traits = CGAL::Projection_traits_3<K>;
  using Vb = CGAL::Triangulation_vertex_base_with_info_2<std::size_t, CDT_traits>;
  using Fb = CGAL::Constrained_triangulation_face_base_2<CDT_traits>;
  using Tds = CGAL::Triangulation_data_structure_2<Vb, Fb>;
  using CDT = CGAL::Constrained_Delaunay_triangulation_2<CDT_traits, Tds>;

 public:
  using Vertex_handle = typename Tds::Vertex_handle;
  using Intersection_of_constraints_exception = typename CDT::Intersection_of_constraints_exception;

  Triangulator(const Triangle& triangle, const std::array<std::size_t, 3>& ids)
      : cdt_{make_cdt_traits(triangle)} {
    const auto& pa = triangle.vertex(0);
    const auto& pb = triangle.vertex(1);
    const auto& pc = triangle.vertex(2);
    insert(pa, ids[0]);
    insert(pb, ids[1]);
    insert(pc, ids[2]);
  }

  template <class OutputIterator>
  void get_triangles(OutputIterator tris) const {
    for (auto it = cdt_.finite_faces_begin(); it != cdt_.finite_faces_end(); ++it) {
      auto a = it->vertex(0)->info();
      auto b = it->vertex(1)->info();
      auto c = it->vertex(2)->info();
      *tris++ = {a, b, c};
    }
  }

  Vertex_handle insert(const Point& p, std::size_t id) {
    auto vh = cdt_.insert(p);
    vh->info() = id;
    return vh;
  }

  void insert_constraint(Vertex_handle vh_i, Vertex_handle vh_j) {
    cdt_.insert_constraint(vh_i, vh_j);
  }

 private:
  CDT_traits make_cdt_traits(const Triangle& triangle) {
    const auto& pa = triangle.vertex(0);
    const auto& pb = triangle.vertex(1);
    const auto& pc = triangle.vertex(2);
    return CDT_traits{CGAL::normal(pa, pb, pc)};
  }

  CDT cdt_;
};

}  // namespace kigumi
