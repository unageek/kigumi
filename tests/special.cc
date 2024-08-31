#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>
#include <kigumi/Boolean_operator.h>
#include <kigumi/Boolean_region_builder.h>
#include <kigumi/Region.h>
#include <kigumi/Triangle_soup.h>

#include <utility>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = K::Point_3;
using Triangle = K::Triangle_3;
using M = kigumi::Region<K>;
using kigumi::Boolean_region_builder;
using kigumi::Boolean_operator;
using kigumi::Triangle_soup;

#define ASSERT_EMPTY(X) ASSERT_TRUE(X.is_empty())
#define ASSERT_FULL(X) ASSERT_TRUE(X.is_full())

TEST(SpecialMeshTest, EmptyEmpty) {
  auto m1 = M::empty();
  auto m2 = M::empty();
  Boolean_region_builder b{m1, m2};

  ASSERT_FULL(b(Boolean_operator::V));
  ASSERT_EMPTY(b(Boolean_operator::A));
  ASSERT_FULL(b(Boolean_operator::B));
  ASSERT_FULL(b(Boolean_operator::C));
  ASSERT_FULL(b(Boolean_operator::D));
  ASSERT_FULL(b(Boolean_operator::E));
  ASSERT_FULL(b(Boolean_operator::F));
  ASSERT_FULL(b(Boolean_operator::G));
  ASSERT_EMPTY(b(Boolean_operator::H));
  ASSERT_EMPTY(b(Boolean_operator::I));
  ASSERT_EMPTY(b(Boolean_operator::J));
  ASSERT_EMPTY(b(Boolean_operator::K));
  ASSERT_EMPTY(b(Boolean_operator::L));
  ASSERT_EMPTY(b(Boolean_operator::M));
  ASSERT_FULL(b(Boolean_operator::X));
  ASSERT_EMPTY(b(Boolean_operator::O));
}

TEST(SpecialMeshTest, EmptyFull) {
  auto m1 = M::empty();
  auto m2 = M::full();
  Boolean_region_builder b{m1, m2};

  ASSERT_FULL(b(Boolean_operator::V));
  ASSERT_FULL(b(Boolean_operator::A));
  ASSERT_EMPTY(b(Boolean_operator::B));
  ASSERT_FULL(b(Boolean_operator::C));
  ASSERT_FULL(b(Boolean_operator::D));
  ASSERT_EMPTY(b(Boolean_operator::E));
  ASSERT_FULL(b(Boolean_operator::F));
  ASSERT_EMPTY(b(Boolean_operator::G));
  ASSERT_FULL(b(Boolean_operator::H));
  ASSERT_EMPTY(b(Boolean_operator::I));
  ASSERT_FULL(b(Boolean_operator::J));
  ASSERT_EMPTY(b(Boolean_operator::K));
  ASSERT_EMPTY(b(Boolean_operator::L));
  ASSERT_FULL(b(Boolean_operator::M));
  ASSERT_EMPTY(b(Boolean_operator::X));
  ASSERT_EMPTY(b(Boolean_operator::O));
}

TEST(SpecialMeshTest, FullEmpty) {
  auto m1 = M::full();
  auto m2 = M::empty();
  Boolean_region_builder b{m1, m2};

  ASSERT_FULL(b(Boolean_operator::V));
  ASSERT_FULL(b(Boolean_operator::A));
  ASSERT_FULL(b(Boolean_operator::B));
  ASSERT_EMPTY(b(Boolean_operator::C));
  ASSERT_FULL(b(Boolean_operator::D));
  ASSERT_EMPTY(b(Boolean_operator::E));
  ASSERT_EMPTY(b(Boolean_operator::F));
  ASSERT_FULL(b(Boolean_operator::G));
  ASSERT_EMPTY(b(Boolean_operator::H));
  ASSERT_FULL(b(Boolean_operator::I));
  ASSERT_FULL(b(Boolean_operator::J));
  ASSERT_EMPTY(b(Boolean_operator::K));
  ASSERT_FULL(b(Boolean_operator::L));
  ASSERT_EMPTY(b(Boolean_operator::M));
  ASSERT_EMPTY(b(Boolean_operator::X));
  ASSERT_EMPTY(b(Boolean_operator::O));
}

TEST(SpecialMeshTest, FullFull) {
  auto m1 = M::full();
  auto m2 = M::full();
  Boolean_region_builder b{m1, m2};

  ASSERT_FULL(b(Boolean_operator::V));
  ASSERT_FULL(b(Boolean_operator::A));
  ASSERT_FULL(b(Boolean_operator::B));
  ASSERT_FULL(b(Boolean_operator::C));
  ASSERT_EMPTY(b(Boolean_operator::D));
  ASSERT_FULL(b(Boolean_operator::E));
  ASSERT_EMPTY(b(Boolean_operator::F));
  ASSERT_EMPTY(b(Boolean_operator::G));
  ASSERT_FULL(b(Boolean_operator::H));
  ASSERT_FULL(b(Boolean_operator::I));
  ASSERT_EMPTY(b(Boolean_operator::J));
  ASSERT_FULL(b(Boolean_operator::K));
  ASSERT_EMPTY(b(Boolean_operator::L));
  ASSERT_EMPTY(b(Boolean_operator::M));
  ASSERT_EMPTY(b(Boolean_operator::X));
  ASSERT_EMPTY(b(Boolean_operator::O));
}

bool is_normal(const M& m) {
  const auto& soup = m.boundary();
  if (soup.num_faces() != 1) {
    return false;
  }

  Triangle t{Point{0, 0, 0}, Point{1, 0, 0}, Point{0, 1, 0}};
  for (auto fh : soup.faces()) {
    return soup.triangle(fh) == t;
  }

  return false;
}

bool is_inverse(const M& m) {
  const auto& soup = m.boundary();
  if (soup.num_faces() != 1) {
    return false;
  }

  Triangle t{Point{0, 0, 0}, Point{0, 1, 0}, Point{1, 0, 0}};
  for (auto fh : soup.faces()) {
    return soup.triangle(fh) == t;
  }

  return false;
}

#define ASSERT_NORMAL(X) ASSERT_TRUE(is_normal(X))
#define ASSERT_INVERSE(X) ASSERT_TRUE(is_inverse(X))

TEST(SpwcialMeshTest, EmptyNormal) {
  Triangle_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  auto m1 = M::empty();
  M m2{std::move(soup)};
  Boolean_region_builder b{m1, m2};

  ASSERT_FULL(b(Boolean_operator::V));
  ASSERT_NORMAL(b(Boolean_operator::A));
  ASSERT_INVERSE(b(Boolean_operator::B));
  ASSERT_FULL(b(Boolean_operator::C));
  ASSERT_FULL(b(Boolean_operator::D));
  ASSERT_INVERSE(b(Boolean_operator::E));
  ASSERT_FULL(b(Boolean_operator::F));
  ASSERT_INVERSE(b(Boolean_operator::G));
  ASSERT_NORMAL(b(Boolean_operator::H));
  ASSERT_EMPTY(b(Boolean_operator::I));
  ASSERT_NORMAL(b(Boolean_operator::J));
  ASSERT_EMPTY(b(Boolean_operator::K));
  ASSERT_EMPTY(b(Boolean_operator::L));
  ASSERT_NORMAL(b(Boolean_operator::M));
  ASSERT_INVERSE(b(Boolean_operator::X));
  ASSERT_EMPTY(b(Boolean_operator::O));
}

TEST(SpwcialMeshTest, NormalEmpty) {
  Triangle_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  M m1{std::move(soup)};
  auto m2 = M::empty();
  Boolean_region_builder b{m1, m2};

  ASSERT_FULL(b(Boolean_operator::V));
  ASSERT_NORMAL(b(Boolean_operator::A));
  ASSERT_FULL(b(Boolean_operator::B));
  ASSERT_INVERSE(b(Boolean_operator::C));
  ASSERT_FULL(b(Boolean_operator::D));
  ASSERT_INVERSE(b(Boolean_operator::E));
  ASSERT_INVERSE(b(Boolean_operator::F));
  ASSERT_FULL(b(Boolean_operator::G));
  ASSERT_EMPTY(b(Boolean_operator::H));
  ASSERT_NORMAL(b(Boolean_operator::I));
  ASSERT_NORMAL(b(Boolean_operator::J));
  ASSERT_EMPTY(b(Boolean_operator::K));
  ASSERT_NORMAL(b(Boolean_operator::L));
  ASSERT_EMPTY(b(Boolean_operator::M));
  ASSERT_INVERSE(b(Boolean_operator::X));
  ASSERT_EMPTY(b(Boolean_operator::O));
}

TEST(SpwcialMeshTest, FullNormal) {
  Triangle_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  auto m1 = M::full();
  M m2{std::move(soup)};
  Boolean_region_builder b{m1, m2};

  ASSERT_FULL(b(Boolean_operator::V));
  ASSERT_FULL(b(Boolean_operator::A));
  ASSERT_FULL(b(Boolean_operator::B));
  ASSERT_NORMAL(b(Boolean_operator::C));
  ASSERT_INVERSE(b(Boolean_operator::D));
  ASSERT_NORMAL(b(Boolean_operator::E));
  ASSERT_EMPTY(b(Boolean_operator::F));
  ASSERT_INVERSE(b(Boolean_operator::G));
  ASSERT_NORMAL(b(Boolean_operator::H));
  ASSERT_FULL(b(Boolean_operator::I));
  ASSERT_INVERSE(b(Boolean_operator::J));
  ASSERT_NORMAL(b(Boolean_operator::K));
  ASSERT_INVERSE(b(Boolean_operator::L));
  ASSERT_EMPTY(b(Boolean_operator::M));
  ASSERT_EMPTY(b(Boolean_operator::X));
  ASSERT_EMPTY(b(Boolean_operator::O));
}

TEST(SpwcialMeshTest, NormalFull) {
  Triangle_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  M m1{std::move(soup)};
  auto m2 = M::full();
  Boolean_region_builder b{m1, m2};

  ASSERT_FULL(b(Boolean_operator::V));
  ASSERT_FULL(b(Boolean_operator::A));
  ASSERT_NORMAL(b(Boolean_operator::B));
  ASSERT_FULL(b(Boolean_operator::C));
  ASSERT_INVERSE(b(Boolean_operator::D));
  ASSERT_NORMAL(b(Boolean_operator::E));
  ASSERT_INVERSE(b(Boolean_operator::F));
  ASSERT_EMPTY(b(Boolean_operator::G));
  ASSERT_FULL(b(Boolean_operator::H));
  ASSERT_NORMAL(b(Boolean_operator::I));
  ASSERT_INVERSE(b(Boolean_operator::J));
  ASSERT_NORMAL(b(Boolean_operator::K));
  ASSERT_EMPTY(b(Boolean_operator::L));
  ASSERT_INVERSE(b(Boolean_operator::M));
  ASSERT_EMPTY(b(Boolean_operator::X));
  ASSERT_EMPTY(b(Boolean_operator::O));
}

TEST(SpwcialMeshTest, Equivalent) {
  Triangle_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  Triangle_soup<K> soup2{soup};

  M m1{std::move(soup)};
  M m2{std::move(soup2)};
  Boolean_region_builder b{m1, m2};

  ASSERT_FULL(b(Boolean_operator::V));
  ASSERT_NORMAL(b(Boolean_operator::A));
  ASSERT_FULL(b(Boolean_operator::B));
  ASSERT_FULL(b(Boolean_operator::C));
  ASSERT_INVERSE(b(Boolean_operator::D));
  ASSERT_FULL(b(Boolean_operator::E));
  ASSERT_INVERSE(b(Boolean_operator::F));
  ASSERT_INVERSE(b(Boolean_operator::G));
  ASSERT_NORMAL(b(Boolean_operator::H));
  ASSERT_NORMAL(b(Boolean_operator::I));
  ASSERT_EMPTY(b(Boolean_operator::J));
  ASSERT_NORMAL(b(Boolean_operator::K));
  ASSERT_EMPTY(b(Boolean_operator::L));
  ASSERT_EMPTY(b(Boolean_operator::M));
  ASSERT_INVERSE(b(Boolean_operator::X));
  ASSERT_EMPTY(b(Boolean_operator::O));
}

TEST(SpwcialMeshTest, Complementary) {
  Triangle_soup<K> soup;
  {
    auto vh1 = soup.add_vertex({0, 0, 0});
    auto vh2 = soup.add_vertex({1, 0, 0});
    auto vh3 = soup.add_vertex({0, 1, 0});
    soup.add_face({vh1, vh2, vh3});
  }

  Triangle_soup<K> soup2;
  {
    auto vh1 = soup2.add_vertex({0, 0, 0});
    auto vh2 = soup2.add_vertex({1, 0, 0});
    auto vh3 = soup2.add_vertex({0, 1, 0});
    soup2.add_face({vh1, vh3, vh2});
  }

  M m1{std::move(soup)};
  M m2{std::move(soup2)};
  Boolean_region_builder b{m1, m2};

  ASSERT_FULL(b(Boolean_operator::V));
  ASSERT_FULL(b(Boolean_operator::A));
  ASSERT_NORMAL(b(Boolean_operator::B));
  ASSERT_INVERSE(b(Boolean_operator::C));
  ASSERT_FULL(b(Boolean_operator::D));
  ASSERT_EMPTY(b(Boolean_operator::E));
  ASSERT_INVERSE(b(Boolean_operator::F));
  ASSERT_NORMAL(b(Boolean_operator::G));
  ASSERT_INVERSE(b(Boolean_operator::H));
  ASSERT_NORMAL(b(Boolean_operator::I));
  ASSERT_FULL(b(Boolean_operator::J));
  ASSERT_EMPTY(b(Boolean_operator::K));
  ASSERT_NORMAL(b(Boolean_operator::L));
  ASSERT_INVERSE(b(Boolean_operator::M));
  ASSERT_EMPTY(b(Boolean_operator::X));
  ASSERT_EMPTY(b(Boolean_operator::O));
}
