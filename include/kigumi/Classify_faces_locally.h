#pragma once

#include <CGAL/Kernel/global_functions.h>
#include <CGAL/enum.h>
#include <CGAL/number_utils.h>
#include <kigumi/Face_tag.h>
#include <kigumi/Mesh_entities.h>
#include <kigumi/Mesh_indices.h>
#include <kigumi/Mixed.h>
#include <kigumi/Propagate_face_tags.h>
#include <kigumi/Warnings.h>

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Classify_faces_locally {
  using Plane_3 = typename K::Plane_3;
  using Propagate_face_tags = Propagate_face_tags<K, FaceData>;
  using Vector_2 = typename K::Vector_2;

 public:
  Warnings operator()(Mixed_triangle_mesh<K, FaceData>& m, const Edge& edge,
                      const Edge_set& border_edges) const {
    bool found_untagged_face{};
    for (auto fi : m.faces_around_edge(edge)) {
      if (m.data(fi).tag == Face_tag::UNKNOWN) {
        found_untagged_face = true;
        break;
      }
    }
    if (!found_untagged_face) {
      return {};
    }

    const auto& p = m.point(edge[0]);
    const auto& q = m.point(edge[1]);
    Plane_3 plane{p, q - p};
    auto u = plane.base1();
    auto v = plane.base2();

    faces_.clear();
    for (auto fi : m.faces_around_edge(edge)) {
      // The face is either pqr or qpr.
      const auto& f = m.face(fi);

      std::size_t i{};  // The index of the vertex p.
      std::size_t j{};  // The index of the vertex q.
      while (f.at(i) != edge[0]) {
        ++i;
      }
      while (f.at(j) != edge[1]) {
        ++j;
      }

      std::size_t k{3 - (i + j)};  // The index of the vertex r.
      const auto& r = m.point(f.at(k));
      Vector_2 r_uv((r - p) * u, (r - p) * v);

      auto orientation = j == (i + 1) % 3 ? CGAL::COUNTERCLOCKWISE  // The face is pqr.
                                          : CGAL::CLOCKWISE;        // The face is qpr.

      faces_.emplace_back(fi, f.at(k), std::move(r_uv), orientation);
    }

    // Sort the faces radially around the edge.

    std::sort(faces_.begin(), faces_.end(), Radial_ordering{});

    // Tag coplanar/opposite faces first.

    for (std::size_t i = 0; i < faces_.size(); ++i) {
      auto j = (i + 1) % faces_.size();
      const auto& f = faces_.at(i);
      auto& f_data = m.data(f.fi);
      const auto& g = faces_.at(j);
      auto& g_data = m.data(g.fi);

      if (f.vi_r == g.vi_r) {
        auto tag = f.orientation == g.orientation ? Face_tag::COPLANAR : Face_tag::OPPOSITE;
        f_data.tag = tag;
        g_data.tag = tag;
      }
    }

    // Find pairs of non-overlapping and non-orientable faces and tag them.

    auto is_undefined_configuration = true;
    // At the end of the loop, the kth face is tagged as either interior or exterior.
    std::size_t k{};
    for (std::size_t i = 0; i < faces_.size(); ++i) {
      auto j = (i + 1) % faces_.size();
      const auto& f = faces_.at(i);
      auto& f_data = m.data(f.fi);
      const auto& g = faces_.at(j);
      auto& g_data = m.data(g.fi);

      if (f_data.tag == Face_tag::COPLANAR || f_data.tag == Face_tag::OPPOSITE ||
          g_data.tag == Face_tag::COPLANAR || g_data.tag == Face_tag::OPPOSITE) {
        continue;
      }

      // Neither f nor g is overlapping with adjacent faces.

      if (f.orientation == g.orientation) {
        if (f.orientation == CGAL::COUNTERCLOCKWISE) {
          f_data.tag = Face_tag::INTERIOR;
          g_data.tag = Face_tag::EXTERIOR;
        } else {
          f_data.tag = Face_tag::EXTERIOR;
          g_data.tag = Face_tag::INTERIOR;
        }
        is_undefined_configuration = false;
        k = j;
      }
    }

    if (!is_undefined_configuration) {
      // Check consistency and tag rest of the faces.

      // An example case of an inconsistent configuration:
      //
      //                 Right, Int.
      //                    //|
      //                    //|
      //                    //|
      //             /////////|/////////
      //   Left, ??? ---------+--------- Left, Ext.
      //          :
      //   Should be tagged as interior according to global classification.

      auto consistent = true;
      for (auto dry_run : {true, false}) {
        auto tag = m.data(faces_.at(k).fi).tag;
        auto orientation = faces_.at(k).orientation;
        // Go around and return to the starting point to check consistency.
        for (std::size_t i = k + 1; i <= k + faces_.size(); ++i) {
          const auto& f = faces_.at(i % faces_.size());
          auto& f_data = m.data(f.fi);

          if (f.orientation == orientation) {
            tag = tag == Face_tag::EXTERIOR ? Face_tag::INTERIOR : Face_tag::EXTERIOR;
          }
          orientation = f.orientation;

          if (f_data.tag == Face_tag::UNKNOWN) {
            if (!dry_run) {
              f_data.tag = tag;
            }
          } else if ((f_data.tag == Face_tag::EXTERIOR || f_data.tag == Face_tag::INTERIOR) &&
                     f_data.tag != tag) {
            consistent = false;
            break;
          }
        }

        if (!consistent) {
          // Leave unknown faces to the global classifier.
          break;
        }
      }
    }

    // Propagate the tags.

    Warnings warnings{};

    for (const auto& f : faces_) {
      const auto& f_data = m.data(f.fi);
      if (f_data.tag != Face_tag::UNKNOWN) {
        warnings |= propagate_face_tags_(m, border_edges, f.fi);
      }
    }

    return warnings;
  }

 private:
  struct Face_around_edge {
    Face_index fi;
    Vertex_index vi_r;
    Vector_2 r;
    int radial_bin;
    CGAL::Orientation orientation;

    Face_around_edge(Face_index fi, Vertex_index vi_r, Vector_2 r, CGAL::Orientation orientation)
        : fi{fi},
          vi_r{vi_r},
          r{std::move(r)},
          radial_bin{classify_radial_bin()},
          orientation{orientation} {}

   private:
    int classify_radial_bin() const {
      auto u = CGAL::sign(r.x());
      auto v = CGAL::sign(r.y());

      if (u > 0) {
        if (v > 0) {
          return 1;  // 1st quadrant
        }
        if (v < 0) {
          return 7;  // 4th quadrant
        }
        return 0;  // +u
      }

      if (u < 0) {
        if (v > 0) {
          return 3;  // 2nd quadrant
        }
        if (v < 0) {
          return 5;  // 3rd quadrant
        }
        return 4;  // -u
      }

      if (v > 0) {
        return 2;  // +v
      }
      if (v < 0) {
        return 6;  // -v
      }

      throw std::runtime_error("degenerate face");
    }
  };

  struct Radial_ordering {
    bool operator()(const Face_around_edge& f, const Face_around_edge& g) const {
      if (f.vi_r == g.vi_r) {
        return false;
      }
      if (f.radial_bin != g.radial_bin) {
        return f.radial_bin < g.radial_bin;
      }
      return CGAL::orientation(f.r, g.r) > 0;
    }
  };

  mutable std::vector<Face_around_edge> faces_;
  mutable Propagate_face_tags propagate_face_tags_;
};

}  // namespace kigumi
