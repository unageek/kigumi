#pragma once

#include <CGAL/Bbox_3.h>

#include <optional>
#include <utility>

namespace boole {

template <class Kernel>
class Overlap {
  using Bbox = CGAL::Bbox_3;
  using Ray = typename Kernel::Ray_3;
  using Triangle = typename Kernel::Triangle_3;

 public:
  static bool do_intersect(const Bbox& bbox, const Ray& ray) {
    auto p = ray.source();
    auto d = ray.direction();

    std::optional<typename Kernel::FT> t_min;
    std::optional<typename Kernel::FT> t_max;

    for (int i = 0; i < 3; i++) {
      if (d.delta(i) == 0.0) {
        // The ray is parallel to the jk-plane.
        if (p[i] < bbox.min(i) || p[i] > bbox.max(i)) {
          return false;
        }
      } else {
        auto t_i_min = (bbox.min(i) - p[i]) / d.delta(i);
        auto t_i_max = (bbox.max(i) - p[i]) / d.delta(i);

        if (t_i_min > t_i_max) {
          std::swap(t_i_min, t_i_max);
        }

        if (!t_min || t_i_min > *t_min) {
          t_min = t_i_min;
        }
        if (!t_max || t_i_max < *t_max) {
          t_max = t_i_max;
        }
        if (*t_min > *t_max || *t_max < 0) {
          return false;
        }
      }
    }

    return true;
  }

  static bool do_intersect(const Bbox& bbox, const Triangle& triangle) {
    return CGAL::do_overlap(bbox, triangle.bbox());
  }
};

}  // namespace boole
