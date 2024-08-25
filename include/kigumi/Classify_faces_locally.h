#pragma once

#include <CGAL/Kernel/global_functions.h>
#include <CGAL/number_utils.h>
#include <kigumi/Mesh_handles.h>
#include <kigumi/Mesh_items.h>
#include <kigumi/Mixed.h>
#include <kigumi/Propagate_face_tags.h>
#include <kigumi/Warnings.h>

#include <algorithm>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Classify_faces_locally {
  using Plane_3 = typename K::Plane_3;
  using Propagate_face_tags = Propagate_face_tags<K, FaceData>;
  using Vector_2 = typename K::Vector_2;

 public:
  Warnings operator()(Mixed_triangle_mesh<K, FaceData>& m, const Edge& edge,
                      const std::unordered_set<Edge>& border_edges) const {
    bool found_untagged_face{};
    for (auto fh : m.faces_around_edge(edge)) {
      if (m.data(fh).tag == Face_tag::Unknown) {
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
    for (auto fh : m.faces_around_edge(edge)) {
      // The face is either pqr or qpr.
      const auto& f = m.face(fh);

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

      faces_.emplace_back(fh, f.at(k), std::move(r_uv), orientation);
    }

    // Sort the faces radially around the edge.

    std::sort(faces_.begin(), faces_.end(), Radial_ordering{});

    // Tag coplanar/opposite faces first.

    for (std::size_t i = 0; i < faces_.size(); ++i) {
      auto j = (i + 1) % faces_.size();
      const auto& fi = faces_.at(i);
      auto& fi_data = m.data(fi.fh);
      const auto& fj = faces_.at(j);
      auto& fj_data = m.data(fj.fh);

      if (fi.vh_r == fj.vh_r) {
        auto tag = fi.orientation == fj.orientation ? Face_tag::Coplanar : Face_tag::Opposite;
        fi_data.tag = tag;
        fj_data.tag = tag;
      }
    }

    // Find pairs of non-overlapping and non-orientable faces and tag them.

    auto is_undefined_configuration = true;
    // At the end of the loop, the kth face is tagged as either interior or exterior.
    std::size_t k{};
    for (std::size_t i = 0; i < faces_.size(); ++i) {
      auto j = (i + 1) % faces_.size();
      const auto& fi = faces_.at(i);
      auto& fi_data = m.data(fi.fh);
      const auto& fj = faces_.at(j);
      auto& fj_data = m.data(fj.fh);

      if (fi_data.tag == Face_tag::Coplanar || fi_data.tag == Face_tag::Opposite ||
          fj_data.tag == Face_tag::Coplanar || fj_data.tag == Face_tag::Opposite) {
        continue;
      }

      // Neither fi nor fj is overlapping with adjacent faces.

      if (fi.orientation == fj.orientation) {
        if (fi.orientation == CGAL::COUNTERCLOCKWISE) {
          fi_data.tag = Face_tag::Interior;
          fj_data.tag = Face_tag::Exterior;
        } else {
          fi_data.tag = Face_tag::Exterior;
          fj_data.tag = Face_tag::Interior;
        }
        is_undefined_configuration = false;
        k = j;
      }
    }

    if (is_undefined_configuration) {
      return {};
    }

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
      auto tag = m.data(faces_.at(k).fh).tag;
      auto orientation = faces_.at(k).orientation;
      // Go around and return to the starting point to check consistency.
      for (std::size_t i = k + 1; i <= k + faces_.size(); ++i) {
        const auto& f = faces_.at(i % faces_.size());
        auto& f_data = m.data(f.fh);

        if (f.orientation == orientation) {
          tag = tag == Face_tag::Exterior ? Face_tag::Interior : Face_tag::Exterior;
        }
        orientation = f.orientation;

        if (f_data.tag == Face_tag::Unknown) {
          if (!dry_run) {
            f_data.tag = tag;
          }
        } else if ((f_data.tag == Face_tag::Exterior || f_data.tag == Face_tag::Interior) &&
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

    // Propagate face tags.

    Warnings warnings{};

    for (const auto& f : faces_) {
      auto fh = f.fh;
      auto f_tag = m.data(fh).tag;
      if (f_tag == Face_tag::Exterior || f_tag == Face_tag::Interior) {
        warnings |= propagate_face_tags_(m, border_edges, fh);
      }
    }

    return warnings;
  }

 private:
  struct Face_around_edge {
    Face_handle fh;
    Vertex_handle vh_r;
    Vector_2 r;
    int radial_bin;
    CGAL::Orientation orientation;

    Face_around_edge(Face_handle fh, Vertex_handle vh_r, Vector_2 r, CGAL::Orientation orientation)
        : fh{fh},
          vh_r{vh_r},
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
    bool operator()(const Face_around_edge& f1, const Face_around_edge& f2) const {
      if (f1.vh_r == f2.vh_r) {
        return false;
      }
      if (f1.radial_bin != f2.radial_bin) {
        return f1.radial_bin < f2.radial_bin;
      }
      return CGAL::orientation(f1.r, f2.r) > 0;
    }
  };

  mutable std::vector<Face_around_edge> faces_;
  mutable Propagate_face_tags propagate_face_tags_;
};

}  // namespace kigumi
