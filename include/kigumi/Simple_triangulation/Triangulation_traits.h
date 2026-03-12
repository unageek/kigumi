#pragma once

#include <CGAL/Projection_traits_3.h>
#include <CGAL/enum.h>

namespace kigumi {

template <class K>
class Triangulation_traits {
  using Traits = CGAL::Projection_traits_3<K>;

 public:
  using Point = typename Traits::Point_2;

  explicit Triangulation_traits(const typename K::Vector_3& normal) : traits_{normal} {}

  CGAL::Comparison_result compare_x(const Point& a, const Point& b) const {
    return traits_.compare_x_2_object()(traits_.construct_point_2_object()(a),
                                        traits_.construct_point_2_object()(b));
  }

  CGAL::Comparison_result compare_y(const Point& a, const Point& b) const {
    return traits_.compare_y_2_object()(traits_.construct_point_2_object()(a),
                                        traits_.construct_point_2_object()(b));
  }

  CGAL::Orientation orientation(const Point& p, const Point& a, const Point& b) const {
    return traits_.orientation_2_object()(traits_.construct_point_2_object()(p),
                                          traits_.construct_point_2_object()(a),
                                          traits_.construct_point_2_object()(b));
  }

  CGAL::Oriented_side side_of_oriented_circle(const Point& a, const Point& b, const Point& c,
                                              const Point& d) const {
    return traits_.side_of_oriented_circle_2_object()(
        traits_.construct_point_2_object()(a), traits_.construct_point_2_object()(b),
        traits_.construct_point_2_object()(c), traits_.construct_point_2_object()(d));
  }

  Traits traits_;
};

}  // namespace kigumi
