#pragma once

#include <kigumi/Corefine.h>
#include <kigumi/Face_tag_propagator.h>
#include <kigumi/Faces_around_edge_classifier.h>
#include <kigumi/Global_face_classifier.h>
#include <kigumi/Mixed_mesh.h>
#include <kigumi/Polygon_soup.h>
#include <kigumi/Shared_edge_finder.h>

#include <iostream>
#include <iterator>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
Mixed_polygon_soup<K, FaceData> mix(const Polygon_soup<K, FaceData>& left,
                                    const Polygon_soup<K, FaceData>& right) {
  std::cout << "Corefining..." << std::endl;

  Corefine corefine{left, right};

  std::cout << "Constructing mixed mesh..." << std::endl;

  Mixed_mesh<K, FaceData> m;

  std::vector<typename K::Triangle_3> tris;
  for (auto fh : left.faces()) {
    tris.clear();
    corefine.get_left_triangles(fh, std::back_inserter(tris));
    for (const auto& tri : tris) {
      auto p = tri.vertex(0);
      auto q = tri.vertex(1);
      auto r = tri.vertex(2);
      auto new_fh = m.add_face({m.add_vertex(p), m.add_vertex(q), m.add_vertex(r)});
      m.data(new_fh).from_left = true;
      m.data(new_fh).data = left.data(fh);
    }
  }

  for (auto fh : right.faces()) {
    tris.clear();
    corefine.get_right_triangles(fh, std::back_inserter(tris));
    for (const auto& tri : tris) {
      auto p = tri.vertex(0);
      auto q = tri.vertex(1);
      auto r = tri.vertex(2);
      auto new_fh = m.add_face({m.add_vertex(p), m.add_vertex(q), m.add_vertex(r)});
      m.data(new_fh).from_left = false;
      m.data(new_fh).data = right.data(fh);
    }
  }

  m.finalize();

  std::cout << "Local classification..." << std::endl;

  Shared_edge_finder shared_edge_finder{m};
  const auto& shared_edges = shared_edge_finder.shared_edges();
  for (const auto& edge : shared_edges) {
    Faces_around_edge_classifier(m, edge);
  }

  Face_tag_propagator{m, shared_edges};

  std::cout << "Global classification..." << std::endl;

  Global_face_classifier{m, shared_edges};

  return m.into_polygon_soup();
}

}  // namespace kigumi
