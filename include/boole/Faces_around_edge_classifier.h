#pragma once

#include <boole/Mixed_mesh.h>

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace boole {

template <class K>
class Faces_around_edge_classifier {
  using Plane_3 = typename K::Plane_3;
  using Vector_2 = typename K::Vector_2;

 public:
  Faces_around_edge_classifier(Mixed_mesh<K>& m, const Edge& edge) {
    const auto& p = m.point(edge[0]);
    const auto& q = m.point(edge[1]);
    Plane_3 plane(p, q - p);
    auto u = plane.base1();
    auto v = plane.base2();

    std::vector<Face_around_edge> faces;

    for (auto fh : m.faces_around_edge(edge)) {
      // The face is either pqr or qpr.
      const auto& f = m.face(fh);

      std::size_t i = 0;  // The index of the vertex p.
      std::size_t j = 0;  // The index of the vertex q.
      while (f.at(i) != edge[0]) {
        ++i;
      }
      while (f.at(j) != edge[1]) {
        ++j;
      }

      std::size_t k = 3 - (i + j);  // The index of the vertex r.
      const auto& r = m.point(f.at(k));
      Vector_2 r_uv((r - p) * u, (r - p) * v);

      auto orientation = j == (i + 1) % 3 ? CGAL::COUNTERCLOCKWISE  // The face is pqr.
                                          : CGAL::CLOCKWISE;        // The face is qpr.

      faces.emplace_back(fh, f.at(k), r_uv, orientation);
    }

    std::sort(faces.begin(), faces.end(), Radial_less{});

    for (std::size_t i = 0; i < faces.size();) {
      auto j = (i + 1) % faces.size();
      const auto& fi = faces.at(i);
      auto& fi_data = m.data(fi.fh);
      const auto& fj = faces.at(j);
      auto& fj_data = m.data(fj.fh);

      if (fi.orientation != fj.orientation && fi.vh_r == fj.vh_r) {
        // The faces are opposite-overlapping, remove them.
        fi_data.tag = Face_tag::Deleted;
        fj_data.tag = Face_tag::Deleted;
        faces.erase(faces.begin() + static_cast<std::ptrdiff_t>(std::max(i, j)));
        faces.erase(faces.begin() + static_cast<std::ptrdiff_t>(std::min(i, j)));
      } else {
        ++i;
      }
    }

    if (faces.empty()) {
      return;
    }

    auto is_undefined_configuration = true;
    for (std::size_t i = 0; i < faces.size(); ++i) {
      auto j = (i + 1) % faces.size();
      const auto& fi = faces.at(i);
      auto& fi_data = m.data(fi.fh);
      const auto& fj = faces.at(j);
      auto& fj_data = m.data(fj.fh);

      if (fi.orientation == fj.orientation) {
        if (fi.orientation == CGAL::COUNTERCLOCKWISE) {
          fi_data.tag = Face_tag::Intersection;
          fj_data.tag = Face_tag::Union;
        } else {
          fi_data.tag = Face_tag::Union;
          fj_data.tag = Face_tag::Intersection;
        }
        is_undefined_configuration = false;
      }
    }

    if (is_undefined_configuration) {
      return;
    }

    bool new_faces_classified = true;
    while (new_faces_classified) {
      new_faces_classified = false;
      for (std::size_t i = 0; i < faces.size(); ++i) {
        auto j = (i + 1) % faces.size();
        const auto& fi = faces.at(i);
        auto& fi_data = m.data(fi.fh);
        const auto& fj = faces.at(j);
        auto& fj_data = m.data(fj.fh);

        if (fi.orientation != fj.orientation) {
          if (fi_data.tag == Face_tag::Unknown && fj_data.tag != Face_tag::Unknown) {
            fi_data.tag = fj_data.tag;
            new_faces_classified = true;
          } else if (fi_data.tag != Face_tag::Unknown && fj_data.tag == Face_tag::Unknown) {
            fj_data.tag = fi_data.tag;
            new_faces_classified = true;
          } else if (fi_data.tag != fj_data.tag) {
            throw std::runtime_error("invalid input meshes");
          }
        }
      }
    }
  }

 private:
  struct Face_around_edge {
    Face_handle fh;
    Vertex_handle vh_r;
    Vector_2 r;
    int radial_bin;
    CGAL::Orientation orientation;

    Face_around_edge(Face_handle fh, Vertex_handle vh_r, const Vector_2& r,
                     CGAL::Orientation orientation)
        : fh(fh), vh_r(vh_r), r(r), radial_bin(classify_radial_bin(r)), orientation(orientation) {}

   private:
    static int classify_radial_bin(const Vector_2& r) {
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

      throw std::runtime_error("degenerated face");
    }
  };

  struct Radial_less {
    bool operator()(const Face_around_edge& f1, const Face_around_edge& f2) const {
      if (f1.vh_r == f2.vh_r) {
        return false;
      }
      if (f1.radial_bin != f2.radial_bin) {
        return f1.radial_bin < f2.radial_bin;
      }
      auto u1 = f1.r.x();
      auto v1 = f1.r.y();
      auto u2 = f2.r.x();
      auto v2 = f2.r.y();
      return u1 * v2 - u2 * v1 > 0;
    }
  };
};

}  // namespace boole
