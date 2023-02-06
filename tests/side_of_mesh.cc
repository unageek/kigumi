#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>
#include <kigumi/Kigumi_mesh.h>

#include <utility>

#include "make_cube.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using M = kigumi::Kigumi_mesh<K>;
using kigumi::Triangle_soup;

TEST(SideOfMeshTest, Normal) {
  auto m = make_cube<K>({0, 0, 0}, {1, 1, 1}, {});

  ASSERT_EQ(m.side_of_mesh({-1, -1, -1}), CGAL::ON_POSITIVE_SIDE);
  ASSERT_EQ(m.side_of_mesh({0, 0, 0}), CGAL::ON_ORIENTED_BOUNDARY);
  ASSERT_EQ(m.side_of_mesh({0.5, 0.5, 0.5}), CGAL::ON_NEGATIVE_SIDE);
  ASSERT_EQ(m.side_of_mesh({1, 1, 1}), CGAL::ON_ORIENTED_BOUNDARY);
  ASSERT_EQ(m.side_of_mesh({2, 2, 2}), CGAL::ON_POSITIVE_SIDE);
}

TEST(SideOfMeshTest, Inverted) {
  auto m = make_cube<K>({0, 0, 0}, {1, 1, 1}, {}, true);

  ASSERT_EQ(m.side_of_mesh({-1, -1, -1}), CGAL::ON_NEGATIVE_SIDE);
  ASSERT_EQ(m.side_of_mesh({0, 0, 0}), CGAL::ON_ORIENTED_BOUNDARY);
  ASSERT_EQ(m.side_of_mesh({0.5, 0.5, 0.5}), CGAL::ON_POSITIVE_SIDE);
  ASSERT_EQ(m.side_of_mesh({1, 1, 1}), CGAL::ON_ORIENTED_BOUNDARY);
  ASSERT_EQ(m.side_of_mesh({2, 2, 2}), CGAL::ON_NEGATIVE_SIDE);
}

TEST(SideOfMeshTest, Empty) {
  auto m = M::empty();

  ASSERT_EQ(m.side_of_mesh({0, 0, 0}), CGAL::ON_POSITIVE_SIDE);
}

TEST(SideOfMeshTest, Entire) {
  auto m = M::entire();

  ASSERT_EQ(m.side_of_mesh({0, 0, 0}), CGAL::ON_NEGATIVE_SIDE);
}

TEST(SideOfMeshTest, PointOnPlane) {
  Triangle_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  auto vh4 = soup.add_vertex({1, 1, 0});
  soup.add_face({vh1, vh2, vh3});
  soup.add_face({vh2, vh4, vh3});

  M m{std::move(soup)};

  ASSERT_EQ(m.side_of_mesh({0.5, 0.5, 0}), CGAL::ON_ORIENTED_BOUNDARY);
}
