#pragma once

#include <CGAL/Bbox_3.h>

template <class K>
class Coincide {
  using Bbox = CGAL::Bbox_3;
  using FT = typename K::FT;
  using Triangle = typename K::Triangle_3;

 public:
  static bool do_intersect(const Bbox& bbox, const Triangle& triangle) {
    auto tri_bbox = triangle.bbox();
    return bbox.xmin() <= tri_bbox.xmin() && bbox.xmax() >= tri_bbox.xmax() &&
           bbox.ymin() <= tri_bbox.ymin() && bbox.ymax() >= tri_bbox.ymax() &&
           bbox.zmin() <= tri_bbox.zmin() && bbox.zmax() >= tri_bbox.zmax();
  }
};
