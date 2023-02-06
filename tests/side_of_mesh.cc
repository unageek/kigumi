#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>
#include <kigumi/Kigumi_mesh.h>

#include "make_cube.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using M = kigumi::Kigumi_mesh<K>;

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
