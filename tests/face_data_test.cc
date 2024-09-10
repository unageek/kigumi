#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/number_utils.h>
#include <gtest/gtest.h>
#include <kigumi/Boolean_operator.h>
#include <kigumi/Boolean_region_builder.h>
#include <kigumi/Region.h>

#include <cmath>
#include <utility>

#include "make_cube.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = K::Point_3;
struct Face_data {
  int i{};
};
using M = kigumi::Region<K, Face_data>;
using kigumi::Boolean_operator;
using kigumi::Boolean_region_builder;

namespace {

std::pair<double, double> get_areas(const M& m) {
  const auto& soup = m.boundary();
  auto area1 = 0.0;
  auto area2 = 0.0;
  for (auto fh : soup.faces()) {
    auto area = std::sqrt(CGAL::to_double(soup.triangle(fh).squared_area()));
    if (soup.data(fh).i == 1) {
      area1 += area;
    } else {
      area2 += area;
    }
  }
  return {area1, area2};
}

}  // namespace

TEST(FaceDataTest, EmptyNormal) {
  auto m1 = M::empty();
  auto m2 = make_cube<K, Face_data>({0, 0, 0}, {1, 1, 1}, {2});
  Boolean_region_builder b{m1, m2};
  auto m = b(Boolean_operator::UNION);
  auto [area1, area2] = get_areas(m);
  ASSERT_EQ(area1, 0.0);
  ASSERT_EQ(area2, 6.0);
}

TEST(FaceDataTest, NormalEmpty) {
  auto m1 = make_cube<K, Face_data>({0, 0, 0}, {1, 1, 1}, {1});
  auto m2 = M::empty();
  Boolean_region_builder b{m1, m2};
  auto m = b(Boolean_operator::UNION);
  auto [area1, area2] = get_areas(m);
  ASSERT_EQ(area1, 6.0);
  ASSERT_EQ(area2, 0.0);
}

TEST(FaceDataTest, FullNormal) {
  auto m1 = M::full();
  auto m2 = make_cube<K, Face_data>({0, 0, 0}, {1, 1, 1}, {2});
  Boolean_region_builder b{m1, m2};
  auto m = b(Boolean_operator::INTERSECTION);
  auto [area1, area2] = get_areas(m);
  ASSERT_EQ(area1, 0.0);
  ASSERT_EQ(area2, 6.0);
}

TEST(FaceDataTest, NormalFull) {
  auto m1 = make_cube<K, Face_data>({0, 0, 0}, {1, 1, 1}, {1});
  auto m2 = M::full();
  Boolean_region_builder b{m1, m2};
  auto m = b(Boolean_operator::INTERSECTION);
  auto [area1, area2] = get_areas(m);
  ASSERT_EQ(area1, 6.0);
  ASSERT_EQ(area2, 0.0);
}

TEST(FaceDataTest, IntersectingPreferFirst) {
  auto m1 = make_cube<K, Face_data>({0, 0, 0}, {1, 1, 1}, {1});
  auto m2 = make_cube<K, Face_data>({0.5, 0, 0}, {1.5, 1, 1}, {2});
  Boolean_region_builder b{m1, m2};
  auto m = b(Boolean_operator::UNION);
  auto [area1, area2] = get_areas(m);
  ASSERT_EQ(area1, 5.0);
  ASSERT_EQ(area2, 3.0);
}

TEST(FaceDataTest, IntersectingPreferSecond) {
  auto m1 = make_cube<K, Face_data>({0, 0, 0}, {1, 1, 1}, {1});
  auto m2 = make_cube<K, Face_data>({0.5, 0, 0}, {1.5, 1, 1}, {2});
  Boolean_region_builder b{m1, m2};
  auto m = b(Boolean_operator::UNION, false);
  auto [area1, area2] = get_areas(m);
  ASSERT_EQ(area1, 3.0);
  ASSERT_EQ(area2, 5.0);
}

TEST(FaceDataTest, NonIntersecting) {
  auto m1 = make_cube<K, Face_data>({0, 0, 0}, {1, 1, 1}, {1});
  auto m2 = make_cube<K, Face_data>({2, 0, 0}, {3, 1, 1}, {2});
  Boolean_region_builder b{m1, m2};
  auto m = b(Boolean_operator::UNION);
  auto [area1, area2] = get_areas(m);
  ASSERT_EQ(area1, 6.0);
  ASSERT_EQ(area2, 6.0);
}
