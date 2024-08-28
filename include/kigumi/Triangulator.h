#pragma once

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Kernel/global_functions.h>
#include <CGAL/Projection_traits_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <kigumi/Triangle_region.h>

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
  using Face_handle = typename CDT::Face_handle;
  using Intersection_of_constraints_exception = typename CDT::Intersection_of_constraints_exception;
  using Vertex_handle = typename CDT::Vertex_handle;

  Triangulator(Triangle_region f, const Point& pa, const Point& pb, const Point& pc, std::size_t a,
               std::size_t b, std::size_t c)
      : f_{f}, cdt_{make_cdt_traits(pa, pb, pc)} {
    vhs_[0] = insert(pa, a);
    vhs_[1] = insert(pb, b);
    vhs_[2] = insert(pc, c);
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

  Vertex_handle insert(const Point& p, std::size_t id, Triangle_region region) {
    Vertex_handle vh;
    region = intersection(region, f_);
    if (region == Triangle_region::LeftEdge01 || region == Triangle_region::RightEdge01) {
      vh = insert_in_edge(p, id, 0);
    } else if (region == Triangle_region::LeftEdge12 || region == Triangle_region::RightEdge12) {
      vh = insert_in_edge(p, id, 1);
    } else if (region == Triangle_region::LeftEdge20 || region == Triangle_region::RightEdge20) {
      vh = insert_in_edge(p, id, 2);
    } else {
      vh = cdt_.insert(p);
    }
    vh->info() = id;
    return vh;
  }

  void insert_constraint(Vertex_handle vh_i, Vertex_handle vh_j) {
    cdt_.insert_constraint(vh_i, vh_j);
  }

 private:
  Vertex_handle insert_in_edge(const Point& p, std::size_t id, int ei) {
    Vertex_handle vh;
    Face_handle fh;
    if (cdt_.is_edge(vhs_.at(ei), vhs_.at((ei + 1) % 3), fh, ei)) {
      vh = cdt_.insert(p, CDT::EDGE, fh, ei);
    } else {
      vh = cdt_.insert(p);
    }
    vh->info() = id;
    return vh;
  }

  CDT_traits make_cdt_traits(const Point& pa, const Point& pb, const Point& pc) {
    return CDT_traits{CGAL::normal(pa, pb, pc)};
  }

  Triangle_region f_{};
  CDT cdt_;
  std::array<Vertex_handle, 3> vhs_;
};

}  // namespace kigumi
