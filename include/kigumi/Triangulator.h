#pragma once

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Projection_traits_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>

namespace kigumi {

template <class K>
class Triangulator {
  using Point_3 = typename K::Point_3;
  using Triangle_3 = typename K::Triangle_3;
  using CDT_traits = CGAL::Projection_traits_3<K>;
  using Vb = CGAL::Triangulation_vertex_base_with_info_2<std::size_t, CDT_traits>;
  using Fb = CGAL::Constrained_triangulation_face_base_2<CDT_traits>;
  using Tds = CGAL::Triangulation_data_structure_2<Vb, Fb>;
  using CDT = CGAL::Constrained_Delaunay_triangulation_2<CDT_traits, Tds>;

 public:
  using Vertex_handle = typename Tds::Vertex_handle;
  using Intersection_of_constraints_exception = typename CDT::Intersection_of_constraints_exception;

  explicit Triangulator(const Triangle_3& triangle, const std::array<std::size_t, 3>& ids)
      : cdt_{make_cdt_traits(triangle)} {
    auto p = triangle.vertex(0);
    auto q = triangle.vertex(1);
    auto r = triangle.vertex(2);
    insert(p, ids[0]);
    insert(q, ids[1]);
    insert(r, ids[2]);
  }

  template <class OutputIterator>
  void get_triangles(OutputIterator tris) const {
    for (auto it = cdt_.finite_faces_begin(); it != cdt_.finite_faces_end(); ++it) {
      auto id0 = it->vertex(0)->info();
      auto id1 = it->vertex(1)->info();
      auto id2 = it->vertex(2)->info();
      *tris++ = {id0, id1, id2};
    }
  }

  Vertex_handle insert(const Point_3& p, std::size_t id) {
    auto vh = cdt_.insert(p);
    vh->info() = id;
    return vh;
  }

  void insert_constraint(Vertex_handle vh_i, Vertex_handle vh_j) {
    cdt_.insert_constraint(vh_i, vh_j);
  }

 private:
  CDT_traits make_cdt_traits(const Triangle_3& triangle) {
    auto p = triangle.vertex(0);
    auto q = triangle.vertex(1);
    auto r = triangle.vertex(2);
    return CDT_traits{CGAL::normal(p, q, r)};
  }

  CDT cdt_;
};

}  // namespace kigumi
