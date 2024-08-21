#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>
#include <kigumi/Classify_faces_locally.h>
#include <kigumi/Mixed.h>

#include <unordered_set>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = K::Point_3;
using kigumi::Edge;
using kigumi::Face_tag;
using Classify_faces_locally = kigumi::Classify_faces_locally<K, kigumi::Null_data>;
using kigumi::make_edge;
using kigumi::Mixed_triangle_mesh;

//                  f1 (Ext.)
//                    //|
//                    //|
//                    //|
//                    //|
//   f2 (Int.) ---------+--------- f0 (Ext.)
//             /////////|/////////
//                    //|
//                    //|
//                    //|
//                  f3 (Int.)
TEST(ClassifyFacesLocallyTest, NonOverlapping) {
  Mixed_triangle_mesh<K> m;
  auto p = m.add_vertex({0, 0, 0});
  auto q = m.add_vertex({0, 0, 1});
  auto r0 = m.add_vertex({1, 0, 0});
  auto r1 = m.add_vertex({0, 1, 0});
  auto r2 = m.add_vertex({-1, 0, 0});
  auto r3 = m.add_vertex({0, -1, 0});
  auto f0 = m.add_face({p, q, r0});
  auto f1 = m.add_face({q, p, r1});
  auto f2 = m.add_face({q, p, r2});
  auto f3 = m.add_face({p, q, r3});
  m.finalize();

  auto pq = make_edge(p, q);
  std::unordered_set<Edge> border{pq};

  Classify_faces_locally{}(m, pq, border);
  ASSERT_EQ(m.data(f0).tag, Face_tag::Exterior);
  ASSERT_EQ(m.data(f1).tag, Face_tag::Exterior);
  ASSERT_EQ(m.data(f2).tag, Face_tag::Interior);
  ASSERT_EQ(m.data(f3).tag, Face_tag::Interior);
}

//   f1 (????)
//     //|
//     //|
//     //|
//     //|
//     //+--------- f0 (????)
//     ////////////
TEST(ClassifyFacesLocallyTest, NonOverlappingUnknown) {
  Mixed_triangle_mesh<K> m;
  auto p = m.add_vertex({0, 0, 0});
  auto q = m.add_vertex({0, 0, 1});
  auto r0 = m.add_vertex({1, 0, 0});
  auto r1 = m.add_vertex({0, 1, 0});
  auto f0 = m.add_face({p, q, r0});
  auto f1 = m.add_face({q, p, r1});
  m.finalize();

  auto pq = make_edge(p, q);
  std::unordered_set<Edge> border{pq};

  Classify_faces_locally{}(m, pq, border);
  ASSERT_EQ(m.data(f0).tag, Face_tag::Unknown);
  ASSERT_EQ(m.data(f1).tag, Face_tag::Unknown);
}

TEST(ClassifyFacesLocallyTest, Coplanar) {
  Mixed_triangle_mesh<K> m;
  auto p = m.add_vertex({0, 0, 0});
  auto q = m.add_vertex({0, 0, 1});
  auto r = m.add_vertex({1, 0, 0});
  auto f0 = m.add_face({p, q, r});
  auto f1 = m.add_face({p, q, r});
  m.finalize();

  auto pq = make_edge(p, q);
  std::unordered_set<Edge> border;

  Classify_faces_locally{}(m, pq, border);
  ASSERT_EQ(m.data(f0).tag, Face_tag::Coplanar);
  ASSERT_EQ(m.data(f1).tag, Face_tag::Coplanar);
}

TEST(ClassifyFacesLocallyTest, Opposite) {
  Mixed_triangle_mesh<K> m;
  auto p = m.add_vertex({0, 0, 0});
  auto q = m.add_vertex({0, 0, 1});
  auto r = m.add_vertex({1, 0, 0});
  auto f0 = m.add_face({p, q, r});
  auto f1 = m.add_face({q, p, r});
  m.finalize();

  auto pq = make_edge(p, q);
  std::unordered_set<Edge> border;

  Classify_faces_locally{}(m, pq, border);
  ASSERT_EQ(m.data(f0).tag, Face_tag::Opposite);
  ASSERT_EQ(m.data(f1).tag, Face_tag::Opposite);
}

//                  f1 (Ext.)
//                    //|
//                    //|
//                    //|
//                    //|
//   f2 (Int.) ---------+--------- f0 (????)
//             ///////////////////
TEST(ClassifyFacesLocallyTest, Inconsistent1) {
  Mixed_triangle_mesh<K> m;
  auto p = m.add_vertex({0, 0, 0});
  auto q = m.add_vertex({0, 0, 1});
  auto r0 = m.add_vertex({1, 0, 0});
  auto r1 = m.add_vertex({0, 1, 0});
  auto r2 = m.add_vertex({-1, 0, 0});
  auto f0 = m.add_face({p, q, r0});
  auto f1 = m.add_face({q, p, r1});
  auto f2 = m.add_face({q, p, r2});
  m.finalize();

  auto pq = make_edge(p, q);
  std::unordered_set<Edge> border{pq};

  Classify_faces_locally{}(m, pq, border);
  EXPECT_EQ(m.data(f0).tag, Face_tag::Unknown);
  EXPECT_EQ(m.data(f1).tag, Face_tag::Exterior);
  EXPECT_EQ(m.data(f2).tag, Face_tag::Interior);
}

//   f1 (Ext.) ---------+--------- f0 (????)
//             /////////|/////////
//                      |//
//                      |//
//                      |//
//                  f2 (Int.)
TEST(ClassifyFacesLocallyTest, Inconsistent2) {
  Mixed_triangle_mesh<K> m;
  auto p = m.add_vertex({0, 0, 0});
  auto q = m.add_vertex({0, 0, 1});
  auto r0 = m.add_vertex({1, 0, 0});
  auto r1 = m.add_vertex({-1, 0, 0});
  auto r2 = m.add_vertex({0, -1, 0});
  auto f0 = m.add_face({p, q, r0});
  auto f1 = m.add_face({q, p, r1});
  auto f2 = m.add_face({q, p, r2});
  m.finalize();

  auto pq = make_edge(p, q);
  std::unordered_set<Edge> border{pq};

  Classify_faces_locally{}(m, pq, border);
  EXPECT_EQ(m.data(f0).tag, Face_tag::Unknown);
  EXPECT_EQ(m.data(f1).tag, Face_tag::Exterior);
  EXPECT_EQ(m.data(f2).tag, Face_tag::Interior);
}
