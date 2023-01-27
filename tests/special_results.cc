#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>

#include "make_cube.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using kigumi::Operator;

auto mk_cube(double length, bool invert = false) {
  auto min = -length / 2.0;
  auto max = length / 2.0;
  return make_cube<K>({min, min, min}, {max, max, max}, {}, invert);
}

TEST(SpecialResultTest, A) {
  {
    auto m1 = mk_cube(1, true);
    auto m2 = mk_cube(2);
    auto m = m1.boolean(m2).apply(Operator::A);
    ASSERT_TRUE(m.is_entire());
  }
  {
    auto m1 = mk_cube(1, true);
    auto m2 = mk_cube(1);
    auto m = m1.boolean(m2).apply(Operator::A);
    ASSERT_TRUE(m.is_entire());
  }
}

TEST(SpecialResultTest, B) {
  {
    auto m1 = mk_cube(2);
    auto m2 = mk_cube(1);
    auto m = m1.boolean(m2).apply(Operator::B);
    ASSERT_TRUE(m.is_entire());
  }
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(1);
    auto m = m1.boolean(m2).apply(Operator::B);
    ASSERT_TRUE(m.is_entire());
  }
}

TEST(SpecialResultTest, C) {
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(2);
    auto m = m1.boolean(m2).apply(Operator::C);
    ASSERT_TRUE(m.is_entire());
  }
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(1);
    auto m = m1.boolean(m2).apply(Operator::C);
    ASSERT_TRUE(m.is_entire());
  }
}

TEST(SpecialResultTest, D) {
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(2, true);
    auto m = m1.boolean(m2).apply(Operator::D);
    ASSERT_TRUE(m.is_entire());
  }
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(1, true);
    auto m = m1.boolean(m2).apply(Operator::D);
    ASSERT_TRUE(m.is_entire());
  }
}

TEST(SpecialResultTest, K) {
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(2, true);
    auto m = m1.boolean(m2).apply(Operator::K);
    ASSERT_TRUE(m.is_empty());
  }
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(1, true);
    auto m = m1.boolean(m2).apply(Operator::K);
    ASSERT_TRUE(m.is_empty());
  }
}

TEST(SpecialResultTest, L) {
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(2);
    auto m = m1.boolean(m2).apply(Operator::L);
    ASSERT_TRUE(m.is_empty());
  }
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(1);
    auto m = m1.boolean(m2).apply(Operator::L);
    ASSERT_TRUE(m.is_empty());
  }
}

TEST(SpecialResultTest, M) {
  {
    auto m1 = mk_cube(2);
    auto m2 = mk_cube(1);
    auto m = m1.boolean(m2).apply(Operator::M);
    ASSERT_TRUE(m.is_empty());
  }
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(1);
    auto m = m1.boolean(m2).apply(Operator::M);
    ASSERT_TRUE(m.is_empty());
  }
}

TEST(SpecialResultTest, X) {
  {
    auto m1 = mk_cube(1, true);
    auto m2 = mk_cube(2);
    auto m = m1.boolean(m2).apply(Operator::X);
    ASSERT_TRUE(m.is_empty());
  }
  {
    auto m1 = mk_cube(1, true);
    auto m2 = mk_cube(1);
    auto m = m1.boolean(m2).apply(Operator::X);
    ASSERT_TRUE(m.is_empty());
  }
}
