#pragma once

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Kernel/global_functions.h>
#include <CGAL/Projection_traits_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <kigumi/Mesh_indices.h>
#include <kigumi/Triangle_region.h>

#include <array>
#include <boost/unordered/unordered_flat_map.hpp>
#include <stdexcept>

namespace kigumi {

template <class K>
class Triangulation {
  using Point = typename K::Point_3;
  using CDT_traits = CGAL::Projection_traits_3<K>;
  using Vb = CGAL::Triangulation_vertex_base_with_info_2<std::size_t, CDT_traits>;
  using Fb = CGAL::Constrained_triangulation_face_base_2<CDT_traits>;
  using Tds = CGAL::Triangulation_data_structure_2<Vb, Fb>;
  using CDT = CGAL::Constrained_Delaunay_triangulation_2<CDT_traits, Tds>;

 public:
  using Face_handle = typename CDT::Face_handle;
  using Intersection_of_constraints_exception = typename CDT::Intersection_of_constraints_exception;
  using Vertex_handle = typename CDT::Vertex_handle;

  Triangulation(Triangle_region f, const Point& pa, const Point& pb, const Point& pc, std::size_t a,
                std::size_t b, std::size_t c)
      : f_{f}, cdt_{make_cdt_traits(pa, pb, pc)} {
    // To keep id_to_vh_ small, we do not insert these vertices into it.
    vhs_[0] = cdt_.insert_outside_affine_hull(pa);
    vhs_[0]->info() = a;
    vhs_[1] = cdt_.insert_outside_affine_hull(pb);
    vhs_[1]->info() = b;
    vhs_[2] = cdt_.insert_outside_affine_hull(pc);
    vhs_[2]->info() = c;
  }

  template <class OutputIterator>
  std::size_t get_faces(OutputIterator faces) const {
    std::size_t count{};
    for (auto it = cdt_.finite_faces_begin(); it != cdt_.finite_faces_end(); ++it) {
      auto a = Vertex_index{it->vertex(0)->info()};
      auto b = Vertex_index{it->vertex(1)->info()};
      auto c = Vertex_index{it->vertex(2)->info()};
      *faces++ = {a, b, c};
      ++count;
    }
    return count;
  }

  Vertex_handle insert(const Point& p, std::size_t id, Triangle_region region) {
    switch (intersection(region, f_)) {
      case Triangle_region::LEFT_VERTEX_0:
      case Triangle_region::RIGHT_VERTEX_0:
        return vhs_[0];

      case Triangle_region::LEFT_VERTEX_1:
      case Triangle_region::RIGHT_VERTEX_1:
        return vhs_[1];

      case Triangle_region::LEFT_VERTEX_2:
      case Triangle_region::RIGHT_VERTEX_2:
        return vhs_[2];

      case Triangle_region::LEFT_EDGE_01:
      case Triangle_region::RIGHT_EDGE_01:
        return insert_in_edge(p, id, 0);

      case Triangle_region::LEFT_EDGE_12:
      case Triangle_region::RIGHT_EDGE_12:
        return insert_in_edge(p, id, 1);

      case Triangle_region::LEFT_EDGE_20:
      case Triangle_region::RIGHT_EDGE_20:
        return insert_in_edge(p, id, 2);

      case Triangle_region::LEFT_FACE:
      case Triangle_region::RIGHT_FACE:
        return insert_in_face(p, id);

      default:
        throw std::runtime_error("invalid region");
    }
  }

  void insert_constraint(Vertex_handle vh_i, Vertex_handle vh_j) {
    cdt_.insert_constraint(vh_i, vh_j);
  }

 private:
  Vertex_handle insert_in_edge(const Point& p, std::size_t id, int ei) {
    auto [it, inserted] = id_to_vh_.emplace(id, Vertex_handle{});

    if (inserted) {
      Vertex_handle vh;
      Face_handle fh;
      if (cdt_.is_edge(vhs_.at(ei), vhs_.at((ei + 1) % 3), fh, ei)) {
        vh = cdt_.insert(p, CDT::EDGE, fh, ei);
      } else {
        vh = cdt_.insert(p);
      }
      vh->info() = id;
      it->second = vh;
    }

    return it->second;
  }

  Vertex_handle insert_in_face(const Point& p, std::size_t id) {
    auto [it, inserted] = id_to_vh_.emplace(id, Vertex_handle{});

    if (inserted) {
      Vertex_handle vh;
      Face_handle fh;
      if (cdt_.is_face(vhs_[0], vhs_[1], vhs_[2], fh)) {
        vh = cdt_.insert(p, CDT::FACE, fh, -1);
      } else {
        vh = cdt_.insert(p);
      }
      vh->info() = id;
      it->second = vh;
    }

    return it->second;
  }

  static CDT_traits make_cdt_traits(const Point& pa, const Point& pb, const Point& pc) {
    return CDT_traits{CGAL::normal(pa, pb, pc)};
  }

  Triangle_region f_{};
  CDT cdt_;
  std::array<Vertex_handle, 3> vhs_;
  boost::unordered_flat_map<std::size_t, Vertex_handle> id_to_vh_;
};

}  // namespace kigumi
