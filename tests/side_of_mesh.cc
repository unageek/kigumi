#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>
#include <kigumi/Kigumi_mesh.h>

#include <utility>

#include "make_cube.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using M = kigumi::Kigumi_mesh<K>;
using kigumi::Triangle_soup;

TEST(SideOfMeshTest, Cube) {
  auto m = make_cube<K>({0, 0, 0}, {1, 1, 1}, {});

  for (auto x : {-1.0, 0.0, 0.5, 1.0, 2.0}) {
    for (auto y : {-1.0, 0.0, 0.5, 1.0, 2.0}) {
      for (auto z : {-1.0, 0.0, 0.5, 1.0, 2.0}) {
        auto p = K::Point_3{x, y, z};
        auto side = m.side_of_mesh(p);

        if (x < 0.0 || y < 0.0 || z < 0.0 || x > 1.0 || y > 1.0 || z > 1.0) {
          ASSERT_EQ(side, CGAL::ON_POSITIVE_SIDE);
        } else if (x == 0.0 || y == 0.0 || z == 0.0 || x == 1.0 || y == 1.0 || z == 1.0) {
          ASSERT_EQ(side, CGAL::ON_ORIENTED_BOUNDARY);
        } else {
          ASSERT_EQ(side, CGAL::ON_NEGATIVE_SIDE);
        }
      }
    }
  }
}

TEST(SideOfMeshTest, InvertedCube) {
  auto m = make_cube<K>({0, 0, 0}, {1, 1, 1}, {}, true);

  for (auto x : {-1.0, 0.0, 0.5, 1.0, 2.0}) {
    for (auto y : {-1.0, 0.0, 0.5, 1.0, 2.0}) {
      for (auto z : {-1.0, 0.0, 0.5, 1.0, 2.0}) {
        auto p = K::Point_3{x, y, z};
        auto side = m.side_of_mesh(p);

        if (x < 0.0 || y < 0.0 || z < 0.0 || x > 1.0 || y > 1.0 || z > 1.0) {
          ASSERT_EQ(side, CGAL::ON_NEGATIVE_SIDE);
        } else if (x == 0.0 || y == 0.0 || z == 0.0 || x == 1.0 || y == 1.0 || z == 1.0) {
          ASSERT_EQ(side, CGAL::ON_ORIENTED_BOUNDARY);
        } else {
          ASSERT_EQ(side, CGAL::ON_POSITIVE_SIDE);
        }
      }
    }
  }
}

TEST(SideOfMeshTest, Empty) {
  auto m = M::empty();

  ASSERT_EQ(m.side_of_mesh({0, 0, 0}), CGAL::ON_POSITIVE_SIDE);
}

TEST(SideOfMeshTest, Entire) {
  auto m = M::entire();

  ASSERT_EQ(m.side_of_mesh({0, 0, 0}), CGAL::ON_NEGATIVE_SIDE);
}

TEST(SideOfMeshTest, Plane) {
  Triangle_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0.5});
  auto vh2 = soup.add_vertex({1, 0, 0.5});
  auto vh3 = soup.add_vertex({0, 1, 0.5});
  auto vh4 = soup.add_vertex({1, 1, 0.5});
  soup.add_face({vh1, vh2, vh3});
  soup.add_face({vh2, vh4, vh3});

  M m{std::move(soup)};

  for (auto x : {0.0, 0.5, 1.0}) {
    for (auto y : {0.0, 0.5, 1.0}) {
      for (auto z : {0.0, 0.5, 1.0}) {
        auto p = K::Point_3{x, y, z};
        auto side = m.side_of_mesh(p);

        if (z < 0.5) {
          ASSERT_EQ(side, CGAL::ON_NEGATIVE_SIDE);
        } else if (z == 0.5) {
          ASSERT_EQ(side, CGAL::ON_ORIENTED_BOUNDARY);
        } else {
          ASSERT_EQ(side, CGAL::ON_POSITIVE_SIDE);
        }
      }
    }
  }
}
