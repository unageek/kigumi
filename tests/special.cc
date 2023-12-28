#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>
#include <kigumi/Kigumi_mesh.h>
#include <kigumi/Triangle_soup.h>

#include <utility>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = K::Point_3;
using Triangle = K::Triangle_3;
using M = kigumi::Kigumi_mesh<K>;
using kigumi::Operator;
using kigumi::Triangle_soup;

#define ASSERT_EMPTY(X) ASSERT_TRUE(X.is_empty())
#define ASSERT_ENTIRE(X) ASSERT_TRUE(X.is_entire())

TEST(SpecialMeshTest, EmptyEmpty) {
  auto m1 = M::empty();
  auto m2 = M::empty();
  auto [b, warnings] = m1.boolean(m2);

  ASSERT_ENTIRE(b.apply(Operator::V));
  ASSERT_EMPTY(b.apply(Operator::A));
  ASSERT_ENTIRE(b.apply(Operator::B));
  ASSERT_ENTIRE(b.apply(Operator::C));
  ASSERT_ENTIRE(b.apply(Operator::D));
  ASSERT_ENTIRE(b.apply(Operator::E));
  ASSERT_ENTIRE(b.apply(Operator::F));
  ASSERT_ENTIRE(b.apply(Operator::G));
  ASSERT_EMPTY(b.apply(Operator::H));
  ASSERT_EMPTY(b.apply(Operator::I));
  ASSERT_EMPTY(b.apply(Operator::J));
  ASSERT_EMPTY(b.apply(Operator::K));
  ASSERT_EMPTY(b.apply(Operator::L));
  ASSERT_EMPTY(b.apply(Operator::M));
  ASSERT_ENTIRE(b.apply(Operator::X));
  ASSERT_EMPTY(b.apply(Operator::O));
}

TEST(SpecialMeshTest, EmptyEntire) {
  auto m1 = M::empty();
  auto m2 = M::entire();
  auto [b, warnings] = m1.boolean(m2);

  ASSERT_ENTIRE(b.apply(Operator::V));
  ASSERT_ENTIRE(b.apply(Operator::A));
  ASSERT_EMPTY(b.apply(Operator::B));
  ASSERT_ENTIRE(b.apply(Operator::C));
  ASSERT_ENTIRE(b.apply(Operator::D));
  ASSERT_EMPTY(b.apply(Operator::E));
  ASSERT_ENTIRE(b.apply(Operator::F));
  ASSERT_EMPTY(b.apply(Operator::G));
  ASSERT_ENTIRE(b.apply(Operator::H));
  ASSERT_EMPTY(b.apply(Operator::I));
  ASSERT_ENTIRE(b.apply(Operator::J));
  ASSERT_EMPTY(b.apply(Operator::K));
  ASSERT_EMPTY(b.apply(Operator::L));
  ASSERT_ENTIRE(b.apply(Operator::M));
  ASSERT_EMPTY(b.apply(Operator::X));
  ASSERT_EMPTY(b.apply(Operator::O));
}

TEST(SpecialMeshTest, EntireEmpty) {
  auto m1 = M::entire();
  auto m2 = M::empty();
  auto [b, warnings] = m1.boolean(m2);

  ASSERT_ENTIRE(b.apply(Operator::V));
  ASSERT_ENTIRE(b.apply(Operator::A));
  ASSERT_ENTIRE(b.apply(Operator::B));
  ASSERT_EMPTY(b.apply(Operator::C));
  ASSERT_ENTIRE(b.apply(Operator::D));
  ASSERT_EMPTY(b.apply(Operator::E));
  ASSERT_EMPTY(b.apply(Operator::F));
  ASSERT_ENTIRE(b.apply(Operator::G));
  ASSERT_EMPTY(b.apply(Operator::H));
  ASSERT_ENTIRE(b.apply(Operator::I));
  ASSERT_ENTIRE(b.apply(Operator::J));
  ASSERT_EMPTY(b.apply(Operator::K));
  ASSERT_ENTIRE(b.apply(Operator::L));
  ASSERT_EMPTY(b.apply(Operator::M));
  ASSERT_EMPTY(b.apply(Operator::X));
  ASSERT_EMPTY(b.apply(Operator::O));
}

TEST(SpecialMeshTest, EntireEntire) {
  auto m1 = M::entire();
  auto m2 = M::entire();
  auto [b, warnings] = m1.boolean(m2);

  ASSERT_ENTIRE(b.apply(Operator::V));
  ASSERT_ENTIRE(b.apply(Operator::A));
  ASSERT_ENTIRE(b.apply(Operator::B));
  ASSERT_ENTIRE(b.apply(Operator::C));
  ASSERT_EMPTY(b.apply(Operator::D));
  ASSERT_ENTIRE(b.apply(Operator::E));
  ASSERT_EMPTY(b.apply(Operator::F));
  ASSERT_EMPTY(b.apply(Operator::G));
  ASSERT_ENTIRE(b.apply(Operator::H));
  ASSERT_ENTIRE(b.apply(Operator::I));
  ASSERT_EMPTY(b.apply(Operator::J));
  ASSERT_ENTIRE(b.apply(Operator::K));
  ASSERT_EMPTY(b.apply(Operator::L));
  ASSERT_EMPTY(b.apply(Operator::M));
  ASSERT_EMPTY(b.apply(Operator::X));
  ASSERT_EMPTY(b.apply(Operator::O));
}

bool is_normal(const M& m) {
  const auto& soup = m.soup();
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
  const auto& soup = m.soup();
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
  auto [b, warnings] = m1.boolean(m2);

  ASSERT_ENTIRE(b.apply(Operator::V));
  ASSERT_NORMAL(b.apply(Operator::A));
  ASSERT_INVERSE(b.apply(Operator::B));
  ASSERT_ENTIRE(b.apply(Operator::C));
  ASSERT_ENTIRE(b.apply(Operator::D));
  ASSERT_INVERSE(b.apply(Operator::E));
  ASSERT_ENTIRE(b.apply(Operator::F));
  ASSERT_INVERSE(b.apply(Operator::G));
  ASSERT_NORMAL(b.apply(Operator::H));
  ASSERT_EMPTY(b.apply(Operator::I));
  ASSERT_NORMAL(b.apply(Operator::J));
  ASSERT_EMPTY(b.apply(Operator::K));
  ASSERT_EMPTY(b.apply(Operator::L));
  ASSERT_NORMAL(b.apply(Operator::M));
  ASSERT_INVERSE(b.apply(Operator::X));
  ASSERT_EMPTY(b.apply(Operator::O));
}

TEST(SpwcialMeshTest, NormalEmpty) {
  Triangle_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  M m1{std::move(soup)};
  auto m2 = M::empty();
  auto [b, warnings] = m1.boolean(m2);

  ASSERT_ENTIRE(b.apply(Operator::V));
  ASSERT_NORMAL(b.apply(Operator::A));
  ASSERT_ENTIRE(b.apply(Operator::B));
  ASSERT_INVERSE(b.apply(Operator::C));
  ASSERT_ENTIRE(b.apply(Operator::D));
  ASSERT_INVERSE(b.apply(Operator::E));
  ASSERT_INVERSE(b.apply(Operator::F));
  ASSERT_ENTIRE(b.apply(Operator::G));
  ASSERT_EMPTY(b.apply(Operator::H));
  ASSERT_NORMAL(b.apply(Operator::I));
  ASSERT_NORMAL(b.apply(Operator::J));
  ASSERT_EMPTY(b.apply(Operator::K));
  ASSERT_NORMAL(b.apply(Operator::L));
  ASSERT_EMPTY(b.apply(Operator::M));
  ASSERT_INVERSE(b.apply(Operator::X));
  ASSERT_EMPTY(b.apply(Operator::O));
}

TEST(SpwcialMeshTest, EntireNormal) {
  Triangle_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  auto m1 = M::entire();
  M m2{std::move(soup)};
  auto [b, warnings] = m1.boolean(m2);

  ASSERT_ENTIRE(b.apply(Operator::V));
  ASSERT_ENTIRE(b.apply(Operator::A));
  ASSERT_ENTIRE(b.apply(Operator::B));
  ASSERT_NORMAL(b.apply(Operator::C));
  ASSERT_INVERSE(b.apply(Operator::D));
  ASSERT_NORMAL(b.apply(Operator::E));
  ASSERT_EMPTY(b.apply(Operator::F));
  ASSERT_INVERSE(b.apply(Operator::G));
  ASSERT_NORMAL(b.apply(Operator::H));
  ASSERT_ENTIRE(b.apply(Operator::I));
  ASSERT_INVERSE(b.apply(Operator::J));
  ASSERT_NORMAL(b.apply(Operator::K));
  ASSERT_INVERSE(b.apply(Operator::L));
  ASSERT_EMPTY(b.apply(Operator::M));
  ASSERT_EMPTY(b.apply(Operator::X));
  ASSERT_EMPTY(b.apply(Operator::O));
}

TEST(SpwcialMeshTest, NormalEntire) {
  Triangle_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  M m1{std::move(soup)};
  auto m2 = M::entire();
  auto [b, warnings] = m1.boolean(m2);

  ASSERT_ENTIRE(b.apply(Operator::V));
  ASSERT_ENTIRE(b.apply(Operator::A));
  ASSERT_NORMAL(b.apply(Operator::B));
  ASSERT_ENTIRE(b.apply(Operator::C));
  ASSERT_INVERSE(b.apply(Operator::D));
  ASSERT_NORMAL(b.apply(Operator::E));
  ASSERT_INVERSE(b.apply(Operator::F));
  ASSERT_EMPTY(b.apply(Operator::G));
  ASSERT_ENTIRE(b.apply(Operator::H));
  ASSERT_NORMAL(b.apply(Operator::I));
  ASSERT_INVERSE(b.apply(Operator::J));
  ASSERT_NORMAL(b.apply(Operator::K));
  ASSERT_EMPTY(b.apply(Operator::L));
  ASSERT_INVERSE(b.apply(Operator::M));
  ASSERT_EMPTY(b.apply(Operator::X));
  ASSERT_EMPTY(b.apply(Operator::O));
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
  auto [b, warnings] = m1.boolean(m2);

  ASSERT_ENTIRE(b.apply(Operator::V));
  ASSERT_NORMAL(b.apply(Operator::A));
  ASSERT_ENTIRE(b.apply(Operator::B));
  ASSERT_ENTIRE(b.apply(Operator::C));
  ASSERT_INVERSE(b.apply(Operator::D));
  ASSERT_ENTIRE(b.apply(Operator::E));
  ASSERT_INVERSE(b.apply(Operator::F));
  ASSERT_INVERSE(b.apply(Operator::G));
  ASSERT_NORMAL(b.apply(Operator::H));
  ASSERT_NORMAL(b.apply(Operator::I));
  ASSERT_EMPTY(b.apply(Operator::J));
  ASSERT_NORMAL(b.apply(Operator::K));
  ASSERT_EMPTY(b.apply(Operator::L));
  ASSERT_EMPTY(b.apply(Operator::M));
  ASSERT_INVERSE(b.apply(Operator::X));
  ASSERT_EMPTY(b.apply(Operator::O));
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
  auto [b, warnings] = m1.boolean(m2);

  ASSERT_ENTIRE(b.apply(Operator::V));
  ASSERT_ENTIRE(b.apply(Operator::A));
  ASSERT_NORMAL(b.apply(Operator::B));
  ASSERT_INVERSE(b.apply(Operator::C));
  ASSERT_ENTIRE(b.apply(Operator::D));
  ASSERT_EMPTY(b.apply(Operator::E));
  ASSERT_INVERSE(b.apply(Operator::F));
  ASSERT_NORMAL(b.apply(Operator::G));
  ASSERT_INVERSE(b.apply(Operator::H));
  ASSERT_NORMAL(b.apply(Operator::I));
  ASSERT_ENTIRE(b.apply(Operator::J));
  ASSERT_EMPTY(b.apply(Operator::K));
  ASSERT_NORMAL(b.apply(Operator::L));
  ASSERT_INVERSE(b.apply(Operator::M));
  ASSERT_EMPTY(b.apply(Operator::X));
  ASSERT_EMPTY(b.apply(Operator::O));
}
