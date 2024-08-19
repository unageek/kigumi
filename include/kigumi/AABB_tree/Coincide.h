#pragma once

#include <CGAL/Bbox_3.h>

namespace kigumi {

template <class K>
class Coincide {
  using Bbox = CGAL::Bbox_3;

 public:
  static bool do_intersect(const Bbox& bbox, const Bbox& query_bbox) { return bbox == query_bbox; }
};

}  // namespace kigumi
