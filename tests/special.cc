#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>
#include <kigumi/Kigumi_mesh.h>
#include <kigumi/Polygon_soup.h>

#include <utility>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = K::Point_3;
using Triangle = K::Triangle_3;
using M = kigumi::Kigumi_mesh<K>;

#define ASSERT_EMPTY(X) ASSERT_TRUE(X.is_empty())
#define ASSERT_ENTIRE(X) ASSERT_TRUE(X.is_entire())

TEST(SpecialMeshTest, EmptyEmpty) {
  using kigumi::Operator;

  auto b = M::boolean(M::empty(), M::empty());
  auto f = [&](Operator op) { return b.apply(op, true, true, true); };

  ASSERT_ENTIRE(f(Operator::V));
  ASSERT_EMPTY(f(Operator::A));
  ASSERT_ENTIRE(f(Operator::B));
  ASSERT_ENTIRE(f(Operator::C));
  ASSERT_ENTIRE(f(Operator::D));
  ASSERT_ENTIRE(f(Operator::E));
  ASSERT_ENTIRE(f(Operator::F));
  ASSERT_ENTIRE(f(Operator::G));
  ASSERT_EMPTY(f(Operator::H));
  ASSERT_EMPTY(f(Operator::I));
  ASSERT_EMPTY(f(Operator::J));
  ASSERT_EMPTY(f(Operator::K));
  ASSERT_EMPTY(f(Operator::L));
  ASSERT_EMPTY(f(Operator::M));
  ASSERT_ENTIRE(f(Operator::X));
  ASSERT_EMPTY(f(Operator::O));
}

TEST(SpecialMeshTest, EmptyEntire) {
  using kigumi::Operator;

  auto b = M::boolean(M::empty(), M::entire());
  auto f = [&](Operator op) { return b.apply(op, true, true, true); };

  ASSERT_ENTIRE(f(Operator::V));
  ASSERT_ENTIRE(f(Operator::A));
  ASSERT_EMPTY(f(Operator::B));
  ASSERT_ENTIRE(f(Operator::C));
  ASSERT_ENTIRE(f(Operator::D));
  ASSERT_EMPTY(f(Operator::E));
  ASSERT_ENTIRE(f(Operator::F));
  ASSERT_EMPTY(f(Operator::G));
  ASSERT_ENTIRE(f(Operator::H));
  ASSERT_EMPTY(f(Operator::I));
  ASSERT_ENTIRE(f(Operator::J));
  ASSERT_EMPTY(f(Operator::K));
  ASSERT_EMPTY(f(Operator::L));
  ASSERT_ENTIRE(f(Operator::M));
  ASSERT_EMPTY(f(Operator::X));
  ASSERT_EMPTY(f(Operator::O));
}

TEST(SpecialMeshTest, EntireEmpty) {
  using kigumi::Operator;

  auto b = M::boolean(M::entire(), M::empty());
  auto f = [&](Operator op) { return b.apply(op, true, true, true); };

  ASSERT_ENTIRE(f(Operator::V));
  ASSERT_ENTIRE(f(Operator::A));
  ASSERT_ENTIRE(f(Operator::B));
  ASSERT_EMPTY(f(Operator::C));
  ASSERT_ENTIRE(f(Operator::D));
  ASSERT_EMPTY(f(Operator::E));
  ASSERT_EMPTY(f(Operator::F));
  ASSERT_ENTIRE(f(Operator::G));
  ASSERT_EMPTY(f(Operator::H));
  ASSERT_ENTIRE(f(Operator::I));
  ASSERT_ENTIRE(f(Operator::J));
  ASSERT_EMPTY(f(Operator::K));
  ASSERT_ENTIRE(f(Operator::L));
  ASSERT_EMPTY(f(Operator::M));
  ASSERT_EMPTY(f(Operator::X));
  ASSERT_EMPTY(f(Operator::O));
}

TEST(SpecialMeshTest, EntireEntire) {
  using kigumi::Operator;

  auto b = M::boolean(M::entire(), M::entire());
  auto f = [&](Operator op) { return b.apply(op, true, true, true); };

  ASSERT_ENTIRE(f(Operator::V));
  ASSERT_ENTIRE(f(Operator::A));
  ASSERT_ENTIRE(f(Operator::B));
  ASSERT_ENTIRE(f(Operator::C));
  ASSERT_EMPTY(f(Operator::D));
  ASSERT_ENTIRE(f(Operator::E));
  ASSERT_EMPTY(f(Operator::F));
  ASSERT_EMPTY(f(Operator::G));
  ASSERT_ENTIRE(f(Operator::H));
  ASSERT_ENTIRE(f(Operator::I));
  ASSERT_EMPTY(f(Operator::J));
  ASSERT_ENTIRE(f(Operator::K));
  ASSERT_EMPTY(f(Operator::L));
  ASSERT_EMPTY(f(Operator::M));
  ASSERT_EMPTY(f(Operator::X));
  ASSERT_EMPTY(f(Operator::O));
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
  using kigumi::Operator;
  using kigumi::Polygon_soup;

  Polygon_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  auto b = M::boolean(M::empty(), M{std::move(soup)});
  auto f = [&](Operator op) { return b.apply(op, true, true, true); };

  ASSERT_ENTIRE(f(Operator::V));
  ASSERT_NORMAL(f(Operator::A));
  ASSERT_INVERSE(f(Operator::B));
  ASSERT_ENTIRE(f(Operator::C));
  ASSERT_ENTIRE(f(Operator::D));
  ASSERT_INVERSE(f(Operator::E));
  ASSERT_ENTIRE(f(Operator::F));
  ASSERT_INVERSE(f(Operator::G));
  ASSERT_NORMAL(f(Operator::H));
  ASSERT_EMPTY(f(Operator::I));
  ASSERT_NORMAL(f(Operator::J));
  ASSERT_EMPTY(f(Operator::K));
  ASSERT_EMPTY(f(Operator::L));
  ASSERT_NORMAL(f(Operator::M));
  ASSERT_INVERSE(f(Operator::X));
  ASSERT_EMPTY(f(Operator::O));
}

TEST(SpwcialMeshTest, NormalEmpty) {
  using kigumi::Operator;
  using kigumi::Polygon_soup;

  Polygon_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  auto b = M::boolean(M{std::move(soup)}, M::empty());
  auto f = [&](Operator op) { return b.apply(op, true, true, true); };

  ASSERT_ENTIRE(f(Operator::V));
  ASSERT_NORMAL(f(Operator::A));
  ASSERT_ENTIRE(f(Operator::B));
  ASSERT_INVERSE(f(Operator::C));
  ASSERT_ENTIRE(f(Operator::D));
  ASSERT_INVERSE(f(Operator::E));
  ASSERT_INVERSE(f(Operator::F));
  ASSERT_ENTIRE(f(Operator::G));
  ASSERT_EMPTY(f(Operator::H));
  ASSERT_NORMAL(f(Operator::I));
  ASSERT_NORMAL(f(Operator::J));
  ASSERT_EMPTY(f(Operator::K));
  ASSERT_NORMAL(f(Operator::L));
  ASSERT_EMPTY(f(Operator::M));
  ASSERT_INVERSE(f(Operator::X));
  ASSERT_EMPTY(f(Operator::O));
}

TEST(SpwcialMeshTest, EntireNormal) {
  using kigumi::Operator;
  using kigumi::Polygon_soup;

  Polygon_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  auto b = M::boolean(M::entire(), M{std::move(soup)});
  auto f = [&](Operator op) { return b.apply(op, true, true, true); };

  ASSERT_ENTIRE(f(Operator::V));
  ASSERT_ENTIRE(f(Operator::A));
  ASSERT_ENTIRE(f(Operator::B));
  ASSERT_NORMAL(f(Operator::C));
  ASSERT_INVERSE(f(Operator::D));
  ASSERT_NORMAL(f(Operator::E));
  ASSERT_EMPTY(f(Operator::F));
  ASSERT_INVERSE(f(Operator::G));
  ASSERT_NORMAL(f(Operator::H));
  ASSERT_ENTIRE(f(Operator::I));
  ASSERT_INVERSE(f(Operator::J));
  ASSERT_NORMAL(f(Operator::K));
  ASSERT_INVERSE(f(Operator::L));
  ASSERT_EMPTY(f(Operator::M));
  ASSERT_EMPTY(f(Operator::X));
  ASSERT_EMPTY(f(Operator::O));
}

TEST(SpwcialMeshTest, NormalEntire) {
  using kigumi::Operator;
  using kigumi::Polygon_soup;

  Polygon_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  auto b = M::boolean(M{std::move(soup)}, M::entire());
  auto f = [&](Operator op) { return b.apply(op, true, true, true); };

  ASSERT_ENTIRE(f(Operator::V));
  ASSERT_ENTIRE(f(Operator::A));
  ASSERT_NORMAL(f(Operator::B));
  ASSERT_ENTIRE(f(Operator::C));
  ASSERT_INVERSE(f(Operator::D));
  ASSERT_NORMAL(f(Operator::E));
  ASSERT_INVERSE(f(Operator::F));
  ASSERT_EMPTY(f(Operator::G));
  ASSERT_ENTIRE(f(Operator::H));
  ASSERT_NORMAL(f(Operator::I));
  ASSERT_INVERSE(f(Operator::J));
  ASSERT_NORMAL(f(Operator::K));
  ASSERT_EMPTY(f(Operator::L));
  ASSERT_INVERSE(f(Operator::M));
  ASSERT_EMPTY(f(Operator::X));
  ASSERT_EMPTY(f(Operator::O));
}

TEST(SpwcialMeshTest, Coplanar) {
  using kigumi::Operator;
  using kigumi::Polygon_soup;

  Polygon_soup<K> soup;
  auto vh1 = soup.add_vertex({0, 0, 0});
  auto vh2 = soup.add_vertex({1, 0, 0});
  auto vh3 = soup.add_vertex({0, 1, 0});
  soup.add_face({vh1, vh2, vh3});

  Polygon_soup<K> soup2{soup};

  auto b = M::boolean(M{std::move(soup)}, M{std::move(soup2)});
  auto f = [&](Operator op) { return b.apply(op, true, true, true); };

  ASSERT_ENTIRE(f(Operator::V));
  ASSERT_NORMAL(f(Operator::A));
  ASSERT_ENTIRE(f(Operator::B));
  ASSERT_ENTIRE(f(Operator::C));
  ASSERT_INVERSE(f(Operator::D));
  ASSERT_ENTIRE(f(Operator::E));
  ASSERT_INVERSE(f(Operator::F));
  ASSERT_INVERSE(f(Operator::G));
  ASSERT_NORMAL(f(Operator::H));
  ASSERT_NORMAL(f(Operator::I));
  ASSERT_EMPTY(f(Operator::J));
  ASSERT_NORMAL(f(Operator::K));
  ASSERT_EMPTY(f(Operator::L));
  ASSERT_EMPTY(f(Operator::M));
  ASSERT_INVERSE(f(Operator::X));
  ASSERT_EMPTY(f(Operator::O));
}

TEST(SpwcialMeshTest, Complementary) {
  using kigumi::Operator;
  using kigumi::Polygon_soup;

  Polygon_soup<K> soup;
  {
    auto vh1 = soup.add_vertex({0, 0, 0});
    auto vh2 = soup.add_vertex({1, 0, 0});
    auto vh3 = soup.add_vertex({0, 1, 0});
    soup.add_face({vh1, vh2, vh3});
  }

  Polygon_soup<K> soup2;
  {
    auto vh1 = soup2.add_vertex({0, 0, 0});
    auto vh2 = soup2.add_vertex({1, 0, 0});
    auto vh3 = soup2.add_vertex({0, 1, 0});
    soup2.add_face({vh1, vh3, vh2});
  }

  auto b = M::boolean(M{std::move(soup)}, M{std::move(soup2)});
  auto f = [&](Operator op) { return b.apply(op, true, true, true); };

  ASSERT_ENTIRE(f(Operator::V));
  ASSERT_ENTIRE(f(Operator::A));  // FAILS
  ASSERT_NORMAL(f(Operator::B));
  ASSERT_INVERSE(f(Operator::C));
  ASSERT_ENTIRE(f(Operator::D));
  ASSERT_EMPTY(f(Operator::E));  // FAILS
  ASSERT_INVERSE(f(Operator::F));
  ASSERT_NORMAL(f(Operator::G));
  ASSERT_INVERSE(f(Operator::H));
  ASSERT_NORMAL(f(Operator::I));
  ASSERT_ENTIRE(f(Operator::J));  // FAILS
  ASSERT_EMPTY(f(Operator::K));
  ASSERT_NORMAL(f(Operator::L));
  ASSERT_INVERSE(f(Operator::M));
  ASSERT_EMPTY(f(Operator::X));  // FAILS
  ASSERT_EMPTY(f(Operator::O));
}
