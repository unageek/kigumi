#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>
#include <kigumi/Faces_around_edge_classifier.h>
#include <kigumi/Mixed.h>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = K::Point_3;
using kigumi::Face_tag;
using kigumi::Faces_around_edge_classifier;
using kigumi::Mixed_triangle_mesh;

TEST(FacesAroundEdgeClassfierTest, NonOverlapping) {
  Mixed_triangle_mesh<K> m;
  auto p = m.add_vertex({0, 0, 0});
  auto q = m.add_vertex({0, 0, 1});
  auto r0 = m.add_vertex({1, 0, 0});
  auto r1 = m.add_vertex({0, 1, 0});
  auto f0 = m.add_face({p, q, r0});
  auto f1 = m.add_face({p, q, r1});
  m.finalize();
  Faces_around_edge_classifier{m, {p, q}};
  ASSERT_EQ(m.data(f0).tag, Face_tag::Intersection);
  ASSERT_EQ(m.data(f1).tag, Face_tag::Union);
}

TEST(FacesAroundEdgeClassfierTest, NonOverlappingUnknown) {
  Mixed_triangle_mesh<K> m;
  auto p = m.add_vertex({0, 0, 0});
  auto q = m.add_vertex({0, 0, 1});
  auto r0 = m.add_vertex({1, 0, 0});
  auto r1 = m.add_vertex({0, 1, 0});
  auto f0 = m.add_face({p, q, r0});
  auto f1 = m.add_face({q, p, r1});
  m.finalize();
  Faces_around_edge_classifier{m, {p, q}};
  ASSERT_EQ(m.data(f0).tag, Face_tag::Unknown);
  ASSERT_EQ(m.data(f1).tag, Face_tag::Unknown);
}

TEST(FacesAroundEdgeClassfierTest, Coplanar) {
  Mixed_triangle_mesh<K> m;
  auto p = m.add_vertex({0, 0, 0});
  auto q = m.add_vertex({0, 0, 1});
  auto r = m.add_vertex({1, 0, 0});
  auto f0 = m.add_face({p, q, r});
  auto f1 = m.add_face({p, q, r});
  m.finalize();
  Faces_around_edge_classifier{m, {p, q}};
  ASSERT_EQ(m.data(f0).tag, Face_tag::Coplanar);
  ASSERT_EQ(m.data(f1).tag, Face_tag::Coplanar);
}

TEST(FacesAroundEdgeClassfierTest, Opposite) {
  Mixed_triangle_mesh<K> m;
  auto p = m.add_vertex({0, 0, 0});
  auto q = m.add_vertex({0, 0, 1});
  auto r = m.add_vertex({1, 0, 0});
  auto f0 = m.add_face({p, q, r});
  auto f1 = m.add_face({q, p, r});
  m.finalize();
  Faces_around_edge_classifier{m, {p, q}};
  ASSERT_EQ(m.data(f0).tag, Face_tag::Opposite);
  ASSERT_EQ(m.data(f1).tag, Face_tag::Opposite);
}
