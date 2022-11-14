#pragma once

#include <boole/Corefine.h>
#include <boole/Face_tag_propagator.h>
#include <boole/Faces_around_edge_classifier.h>
#include <boole/Global_face_classifier.h>
#include <boole/Label_connected_components.h>
#include <boole/Mixed_mesh.h>
#include <boole/Polygon_soup.h>
#include <boole/Shared_edge_finder.h>
#include <boole/extract.h>

#include <iostream>
#include <iterator>
#include <vector>

namespace boole {

template <class K>
struct Boolean_result {
  Polygon_soup<K> the_intersection;
  Polygon_soup<K> the_union;
};

template <class K>
Boolean_result<K> boolean(const Polygon_soup<K>& left, const Polygon_soup<K>& right) {
  std::cout << "Corefining..." << std::endl;

  Corefine corefine(left, right);

  std::cout << "Constructing mixed mesh..." << std::endl;

  Mixed_mesh<K> m;

  std::vector<typename K::Triangle_3> tris;
  corefine.get_left_triangles(std::back_inserter(tris));
  for (const auto& tri : tris) {
    auto p = tri.vertex(0);
    auto q = tri.vertex(1);
    auto r = tri.vertex(2);
    auto fh = m.add_face({m.add_vertex(p), m.add_vertex(q), m.add_vertex(r)});
    m.data(fh).from_left = true;
  }

  tris.clear();
  corefine.get_right_triangles(std::back_inserter(tris));
  for (const auto& tri : tris) {
    auto p = tri.vertex(0);
    auto q = tri.vertex(1);
    auto r = tri.vertex(2);
    auto fh = m.add_face({m.add_vertex(p), m.add_vertex(q), m.add_vertex(r)});
    m.data(fh).from_left = false;
  }

  m.finalize();

  Label_connected_components label_connected_components{m};
  const auto& representative_faces = label_connected_components.representative_faces();

  std::cout << "Local classification..." << std::endl;

  Shared_edge_finder shared_edge_finder(m);
  const auto& shared_edges = shared_edge_finder.shared_edges();
  for (const auto& edge : shared_edges) {
    Faces_around_edge_classifier(m, edge);
  }

  Face_tag_propagator{m};

  std::cout << "Global classification..." << std::endl;

  Global_face_classifier{m, representative_faces};

  std::cout << "Extracting..." << std::endl;

  return {.the_intersection = extract(m, Face_tag::Intersection),
          .the_union = extract(m, Face_tag::Union)};
}

template <class K>
Polygon_soup<K> operator!(const Polygon_soup<K>& soup) {
  Polygon_soup<K> inverse{soup};
  inverse.invert();
  return inverse;
}

template <class K>
Polygon_soup<K> operator+(const Polygon_soup<K>& left, const Polygon_soup<K>& right) {
  return boolean(left, right).the_union;
}

template <class K>
Polygon_soup<K> operator*(const Polygon_soup<K>& left, const Polygon_soup<K>& right) {
  return boolean(left, right).the_intersection;
}

template <class K>
Polygon_soup<K> operator-(const Polygon_soup<K>& left, const Polygon_soup<K>& right) {
  return left * !right;
}

}  // namespace boole
