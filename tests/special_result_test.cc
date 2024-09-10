#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>
#include <kigumi/Boolean_operator.h>
#include <kigumi/Boolean_region_builder.h>

#include "make_cube.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using kigumi::Boolean_operator;
using kigumi::Boolean_region_builder;

auto mk_cube(double length, bool invert = false) {
  auto min = -length / 2.0;
  auto max = length / 2.0;
  return make_cube<K>({min, min, min}, {max, max, max}, {}, invert);
}

TEST(SpecialResultTest, A) {
  {
    auto m1 = mk_cube(1, true);
    auto m2 = mk_cube(2);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::A);
    ASSERT_TRUE(m.is_full());
  }
  {
    auto m1 = mk_cube(1, true);
    auto m2 = mk_cube(1);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::A);
    ASSERT_TRUE(m.is_full());
  }
}

TEST(SpecialResultTest, B) {
  {
    auto m1 = mk_cube(2);
    auto m2 = mk_cube(1);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::B);
    ASSERT_TRUE(m.is_full());
  }
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(1);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::B);
    ASSERT_TRUE(m.is_full());
  }
}

TEST(SpecialResultTest, C) {
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(2);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::C);
    ASSERT_TRUE(m.is_full());
  }
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(1);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::C);
    ASSERT_TRUE(m.is_full());
  }
}

TEST(SpecialResultTest, D) {
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(2, true);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::D);
    ASSERT_TRUE(m.is_full());
  }
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(1, true);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::D);
    ASSERT_TRUE(m.is_full());
  }
}

TEST(SpecialResultTest, K) {
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(2, true);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::K);
    ASSERT_TRUE(m.is_empty());
  }
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(1, true);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::K);
    ASSERT_TRUE(m.is_empty());
  }
}

TEST(SpecialResultTest, L) {
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(2);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::L);
    ASSERT_TRUE(m.is_empty());
  }
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(1);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::L);
    ASSERT_TRUE(m.is_empty());
  }
}

TEST(SpecialResultTest, M) {
  {
    auto m1 = mk_cube(2);
    auto m2 = mk_cube(1);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::M);
    ASSERT_TRUE(m.is_empty());
  }
  {
    auto m1 = mk_cube(1);
    auto m2 = mk_cube(1);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::M);
    ASSERT_TRUE(m.is_empty());
  }
}

TEST(SpecialResultTest, X) {
  {
    auto m1 = mk_cube(1, true);
    auto m2 = mk_cube(2);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::X);
    ASSERT_TRUE(m.is_empty());
  }
  {
    auto m1 = mk_cube(1, true);
    auto m2 = mk_cube(1);
    Boolean_region_builder b{m1, m2};
    auto m = b(Boolean_operator::X);
    ASSERT_TRUE(m.is_empty());
  }
}
