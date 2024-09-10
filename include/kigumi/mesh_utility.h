#pragma once

#include <CGAL/Bbox_3.h>
#include <CGAL/Kernel/global_functions.h>
#include <CGAL/enum.h>
#include <kigumi/Mesh_indices.h>

namespace kigumi {

template <class K, class FaceData>
class Triangle_soup;

template <class K, class FaceData>
class Triangle_mesh;

namespace internal {

// Facilities for avoiding construction of intermediate kernel objects.

template <class K, class FaceData>
CGAL::Bbox_3 face_bbox(const Triangle_soup<K, FaceData>& m, Face_index fi) {
  auto bbox_fast = [](const typename K::Point_3& p) -> CGAL::Bbox_3 { return p.approx().bbox(); };

  const auto& f = m.face(fi);
  return bbox_fast(m.point(f[0])) + bbox_fast(m.point(f[1])) + bbox_fast(m.point(f[2]));
}

template <class K, class FaceData>
typename K::Point_3 face_centroid(const Triangle_soup<K, FaceData>& m, Face_index fi) {
  const auto& f = m.face(fi);
  return CGAL::centroid(m.point(f[0]), m.point(f[1]), m.point(f[2]));
}

template <class K, class FaceData>
typename K::Point_3 face_centroid(const Triangle_mesh<K, FaceData>& m, Face_index fi) {
  const auto& f = m.face(fi);
  return CGAL::centroid(m.point(f[0]), m.point(f[1]), m.point(f[2]));
}

template <class K, class FaceData>
CGAL::Oriented_side oriented_side_of_face_supporting_plane(const Triangle_soup<K, FaceData>& m,
                                                           Face_index fi,
                                                           const typename K::Point_3& p) {
  const auto& f = m.face(fi);
  return CGAL::orientation(m.point(f[0]), m.point(f[1]), m.point(f[2]), p);
}

}  // namespace internal

}  // namespace kigumi
