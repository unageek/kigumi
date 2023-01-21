#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <gtest/gtest.h>
#include <kigumi/Kigumi_mesh.h>
#include <kigumi/Polygon_soup.h>

#include <algorithm>
#include <utility>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = K::Point_3;

struct Face_data {
  int i{};
};

TEST(FaceDataTest, Intersecting) {
  using namespace kigumi;

  Polygon_soup<K, Face_data> s1;
  {
    auto vh1 = s1.add_vertex({0, 0, 1});
    auto vh2 = s1.add_vertex({1, 0, 0});
    auto vh3 = s1.add_vertex({0, 1, 1});
    auto fh = s1.add_face({vh1, vh2, vh3});
    s1.data(fh).i = 1;
  }
  Kigumi_mesh<K, Face_data> m1(std::move(s1));

  Polygon_soup<K, Face_data> s2;
  {
    auto vh1 = s2.add_vertex({0, 0, 0});
    auto vh2 = s2.add_vertex({1, 0, 1});
    auto vh3 = s2.add_vertex({1, 1, 1});
    auto fh = s2.add_face({vh1, vh2, vh3});
    s2.data(fh).i = 2;
  }
  Kigumi_mesh<K, Face_data> m2(std::move(s2));

  auto m = Kigumi_mesh<K, Face_data>::boolean(m1, m2).apply(Operator::Union, true, true, true);
  const auto& soup = m.soup();
  ASSERT_EQ(std::count_if(soup.faces_begin(), soup.faces_end(),
                          [&](auto fh) { return soup.data(fh).i == 1; }),
            2);
  ASSERT_EQ(std::count_if(soup.faces_begin(), soup.faces_end(),
                          [&](auto fh) { return soup.data(fh).i == 2; }),
            2);
}

TEST(FaceDataTest, NonIntersecting) {
  using namespace kigumi;

  Polygon_soup<K, Face_data> s1;
  {
    auto vh1 = s1.add_vertex({0, 0, 1});
    auto vh2 = s1.add_vertex({1, 0, 0});
    auto vh3 = s1.add_vertex({0, 1, 1});
    auto fh = s1.add_face({vh1, vh2, vh3});
    s1.data(fh).i = 1;
  }
  Kigumi_mesh<K, Face_data> m1(std::move(s1));

  Polygon_soup<K, Face_data> s2;
  {
    auto vh1 = s2.add_vertex({2, 0, 0});
    auto vh2 = s2.add_vertex({3, 0, 1});
    auto vh3 = s2.add_vertex({3, 1, 1});
    auto fh = s2.add_face({vh1, vh2, vh3});
    s2.data(fh).i = 2;
  }
  Kigumi_mesh<K, Face_data> m2(std::move(s2));

  auto m = Kigumi_mesh<K, Face_data>::boolean(m1, m2).apply(Operator::Union, true, true, true);
  const auto& soup = m.soup();
  ASSERT_EQ(std::count_if(soup.faces_begin(), soup.faces_end(),
                          [&](auto fh) { return soup.data(fh).i == 1; }),
            1);
  ASSERT_EQ(std::count_if(soup.faces_begin(), soup.faces_end(),
                          [&](auto fh) { return soup.data(fh).i == 1; }),
            1);
}
