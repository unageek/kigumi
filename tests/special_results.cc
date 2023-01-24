#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>

#include "make_cube.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using kigumi::Operator;
using Face_data = std::nullptr_t;

TEST(SpecialResultTest, A) {
  auto m1 = make_cube<K, Face_data>({0, 0, 0}, {1, 1, 1}, nullptr, true);
  auto m2 = make_cube<K, Face_data>({2, 0, 0}, {3, 1, 1}, nullptr, true);
  auto m = m1.boolean(m2).apply(Operator::A);
  ASSERT_TRUE(m.is_entire());
}

TEST(SpecialResultTest, B) {
  auto m1 = make_cube<K, Face_data>({0, 0, 0}, {3, 3, 3}, nullptr);
  auto m2 = make_cube<K, Face_data>({1, 1, 1}, {2, 2, 2}, nullptr);
  auto m = m1.boolean(m2).apply(Operator::B);
  ASSERT_TRUE(m.is_entire());
}

TEST(SpecialResultTest, C) {
  auto m1 = make_cube<K, Face_data>({1, 1, 1}, {2, 2, 2}, nullptr);
  auto m2 = make_cube<K, Face_data>({0, 0, 0}, {3, 3, 3}, nullptr);
  auto m = m1.boolean(m2).apply(Operator::C);
  ASSERT_TRUE(m.is_entire());
}

TEST(SpecialResultTest, D) {
  auto m1 = make_cube<K, Face_data>({0, 0, 0}, {1, 1, 1}, nullptr);
  auto m2 = make_cube<K, Face_data>({2, 0, 0}, {3, 1, 1}, nullptr);
  auto m = m1.boolean(m2).apply(Operator::D);
  ASSERT_TRUE(m.is_entire());
}

TEST(SpecialResultTest, K) {
  auto m1 = make_cube<K, Face_data>({0, 0, 0}, {1, 1, 1}, nullptr);
  auto m2 = make_cube<K, Face_data>({2, 0, 0}, {3, 1, 1}, nullptr);
  auto m = m1.boolean(m2).apply(Operator::K);
  ASSERT_TRUE(m.is_empty());
}

TEST(SpecialResultTest, L) {
  auto m1 = make_cube<K, Face_data>({1, 1, 1}, {2, 2, 2}, nullptr);
  auto m2 = make_cube<K, Face_data>({0, 0, 0}, {3, 3, 3}, nullptr);
  auto m = m1.boolean(m2).apply(Operator::L);
  ASSERT_TRUE(m.is_empty());
}

TEST(SpecialResultTest, M) {
  auto m1 = make_cube<K, Face_data>({0, 0, 0}, {3, 3, 3}, nullptr);
  auto m2 = make_cube<K, Face_data>({1, 1, 1}, {2, 2, 2}, nullptr);
  auto m = m1.boolean(m2).apply(Operator::M);
  ASSERT_TRUE(m.is_empty());
}

TEST(SpecialResultTest, X) {
  auto m1 = make_cube<K, Face_data>({0, 0, 0}, {1, 1, 1}, nullptr, true);
  auto m2 = make_cube<K, Face_data>({2, 0, 0}, {3, 1, 1}, nullptr, true);
  auto m = m1.boolean(m2).apply(Operator::X);
  ASSERT_TRUE(m.is_empty());
}
