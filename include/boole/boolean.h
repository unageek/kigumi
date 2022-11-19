#pragma once

#include <boole/Corefine.h>
#include <boole/Face_tag_propagator.h>
#include <boole/Faces_around_edge_classifier.h>
#include <boole/Global_face_classifier.h>
#include <boole/Mixed_mesh.h>
#include <boole/Operator.h>
#include <boole/Polygon_soup.h>
#include <boole/Shared_edge_finder.h>
#include <boole/extract.h>

#include <iostream>
#include <iterator>
#include <vector>

namespace boole {

template <class K>
std::vector<Polygon_soup<K>> boolean(const Polygon_soup<K>& left, const Polygon_soup<K>& right,
                                     const std::vector<Operator>& ops) {
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

  std::cout << "Local classification..." << std::endl;

  Shared_edge_finder shared_edge_finder(m);
  const auto& shared_edges = shared_edge_finder.shared_edges();
  for (const auto& edge : shared_edges) {
    Faces_around_edge_classifier(m, edge);
  }

  Face_tag_propagator{m, shared_edges};

  std::cout << "Global classification..." << std::endl;

  Global_face_classifier{m, shared_edges};

  std::cout << "Extracting..." << std::endl;

  std::vector<Polygon_soup<K>> result;
  result.reserve(ops.size());
  for (auto op : ops) {
    result.push_back(extract(m, op));
  }
  return result;
}

}  // namespace boole
