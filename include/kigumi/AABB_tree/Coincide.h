#pragma once

#include <CGAL/Bbox_3.h>

template <class K>
class Coincide {
  using Bbox = CGAL::Bbox_3;

 public:
  static bool do_intersect(const Bbox& bbox, const Bbox& query_bbox) {
    return bbox.xmin() <= query_bbox.xmin() && bbox.xmax() >= query_bbox.xmax() &&
           bbox.ymin() <= query_bbox.ymin() && bbox.ymax() >= query_bbox.ymax() &&
           bbox.zmin() <= query_bbox.zmin() && bbox.zmax() >= query_bbox.zmax();
  }
};
