#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/enum.h>
#include <gtest/gtest.h>
#include <kigumi/Region.h>

#include <utility>

#include "make_cube.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using M = kigumi::Region<K>;
using kigumi::Triangle_soup;

TEST(BoundedSideTest, Cube) {
  auto m = make_cube<K>({0, 0, 0}, {1, 1, 1}, {});

  for (auto x : {-1.0, 0.0, 0.5, 1.0, 2.0}) {
    for (auto y : {-1.0, 0.0, 0.5, 1.0, 2.0}) {
      for (auto z : {-1.0, 0.0, 0.5, 1.0, 2.0}) {
        auto p = K::Point_3{x, y, z};
        auto side = m.bounded_side(p);

        if (x < 0.0 || y < 0.0 || z < 0.0 || x > 1.0 || y > 1.0 || z > 1.0) {
          ASSERT_EQ(side, CGAL::ON_UNBOUNDED_SIDE);
        } else if (x == 0.0 || y == 0.0 || z == 0.0 || x == 1.0 || y == 1.0 || z == 1.0) {
          ASSERT_EQ(side, CGAL::ON_BOUNDARY);
        } else {
          ASSERT_EQ(side, CGAL::ON_BOUNDED_SIDE);
        }
      }
    }
  }
}

TEST(BoundedSideTest, InvertedCube) {
  auto m = make_cube<K>({0, 0, 0}, {1, 1, 1}, {}, true);

  for (auto x : {-1.0, 0.0, 0.5, 1.0, 2.0}) {
    for (auto y : {-1.0, 0.0, 0.5, 1.0, 2.0}) {
      for (auto z : {-1.0, 0.0, 0.5, 1.0, 2.0}) {
        auto p = K::Point_3{x, y, z};
        auto side = m.bounded_side(p);

        if (x < 0.0 || y < 0.0 || z < 0.0 || x > 1.0 || y > 1.0 || z > 1.0) {
          ASSERT_EQ(side, CGAL::ON_BOUNDED_SIDE);
        } else if (x == 0.0 || y == 0.0 || z == 0.0 || x == 1.0 || y == 1.0 || z == 1.0) {
          ASSERT_EQ(side, CGAL::ON_BOUNDARY);
        } else {
          ASSERT_EQ(side, CGAL::ON_UNBOUNDED_SIDE);
        }
      }
    }
  }
}

TEST(BoundedSideTest, Empty) {
  auto m = M::empty();

  ASSERT_EQ(m.bounded_side({0, 0, 0}), CGAL::ON_UNBOUNDED_SIDE);
}

TEST(BoundedSideTest, Full) {
  auto m = M::full();

  ASSERT_EQ(m.bounded_side({0, 0, 0}), CGAL::ON_BOUNDED_SIDE);
}

TEST(BoundedSideTest, Plane) {
  Triangle_soup<K> soup;
  auto vi1 = soup.add_vertex({0, 0, 0.5});
  auto vi2 = soup.add_vertex({1, 0, 0.5});
  auto vi3 = soup.add_vertex({0, 1, 0.5});
  auto vi4 = soup.add_vertex({1, 1, 0.5});
  soup.add_face({vi1, vi2, vi3});
  soup.add_face({vi2, vi4, vi3});

  M m{std::move(soup)};

  for (auto x : {0.0, 0.5, 1.0}) {
    for (auto y : {0.0, 0.5, 1.0}) {
      for (auto z : {0.0, 0.5, 1.0}) {
        auto p = K::Point_3{x, y, z};
        auto side = m.bounded_side(p);

        if (z < 0.5) {
          ASSERT_EQ(side, CGAL::ON_BOUNDED_SIDE);
        } else if (z == 0.5) {
          ASSERT_EQ(side, CGAL::ON_BOUNDARY);
        } else {
          ASSERT_EQ(side, CGAL::ON_UNBOUNDED_SIDE);
        }
      }
    }
  }
}
