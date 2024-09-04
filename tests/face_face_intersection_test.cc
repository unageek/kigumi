#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/intersections.h>
#include <gtest/gtest.h>
#include <kigumi/Face_face_intersection.h>
#include <kigumi/Intersection_point_inserter.h>
#include <kigumi/Point_list.h>

#include <algorithm>
#include <array>
#include <boost/variant/get.hpp>
#include <vector>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Face_face_intersection = kigumi::Face_face_intersection<K>;
using Intersection_point_inserter = kigumi::Intersection_point_inserter<K>;
using Point = K::Point_3;
using Point_list = kigumi::Point_list<K>;
using Segment = K::Segment_3;
using Triangle = K::Triangle_3;
using kigumi::intersection;
using kigumi::Triangle_region;

namespace {

std::vector<std::array<std::size_t, 3>> make_cube(Point_list& points, const Point& min,
                                                  const Point& max) {
  auto v1 = points.insert({min.x(), min.y(), min.z()});
  auto v2 = points.insert({max.x(), min.y(), min.z()});
  auto v3 = points.insert({min.x(), max.y(), min.z()});
  auto v4 = points.insert({max.x(), max.y(), min.z()});
  auto v5 = points.insert({min.x(), min.y(), max.z()});
  auto v6 = points.insert({max.x(), min.y(), max.z()});
  auto v7 = points.insert({min.x(), max.y(), max.z()});
  auto v8 = points.insert({max.x(), max.y(), max.z()});
  return {
      {v1, v2, v6}, {v1, v3, v4}, {v1, v4, v2}, {v1, v5, v7}, {v1, v6, v5}, {v1, v7, v3},
      {v8, v2, v4}, {v8, v3, v7}, {v8, v4, v3}, {v8, v5, v6}, {v8, v6, v2}, {v8, v7, v5},
  };
}

bool test(Point_list& points, std::array<std::size_t, 3> abc, std::array<std::size_t, 3> pqr) {
  std::sort(abc.begin(), abc.end());
  std::sort(pqr.begin(), pqr.end());

  const auto& pa = points.at(abc[0]);
  const auto& pb = points.at(abc[1]);
  const auto& pc = points.at(abc[2]);
  const auto& pp = points.at(pqr[0]);
  const auto& pq = points.at(pqr[1]);
  const auto& pr = points.at(pqr[2]);

  std::vector<Point> expected;

  auto inter = CGAL::intersection(Triangle{pa, pb, pc}, Triangle{pp, pq, pr});
  if (inter) {
    if (const auto* p = boost::get<Point>(&*inter)) {
      expected.push_back(*p);
    } else if (const auto* s = boost::get<Segment>(&*inter)) {
      expected.push_back(s->source());
      expected.push_back(s->target());
    } else if (const auto* t = boost::get<Triangle>(&*inter)) {
      expected.push_back(t->vertex(0));
      expected.push_back(t->vertex(1));
      expected.push_back(t->vertex(2));
    } else if (const auto* ps = boost::get<std::vector<Point>>(&*inter)) {
      expected = *ps;
    }
  }

  Intersection_point_inserter inserter(points);

  Face_face_intersection face_face{points};
  while (std::next_permutation(abc.begin(), abc.end())) {
    while (std::next_permutation(pqr.begin(), pqr.end())) {
      auto sym_inters = face_face(abc[0], abc[1], abc[2], pqr[0], pqr[1], pqr[2]);
      if (expected.size() != sym_inters.size()) {
        return false;
      }

      for (auto sym_inter : sym_inters) {
        auto left_region = intersection(sym_inter, Triangle_region::LEFT_FACE);
        auto right_region = intersection(sym_inter, Triangle_region::RIGHT_FACE);
        auto id = inserter.insert(left_region, abc[0], abc[1], abc[2], right_region, pqr[0], pqr[1],
                                  pqr[2]);
        const auto& p = points.at(id);
        if (std::find(expected.begin(), expected.end(), p) == expected.end()) {
          return false;
        }
      }
    }
  }

  return true;
}

}  // namespace

TEST(FaceFaceIntersectionTest, NoIntersectionFast) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 3.0, 0.0}),
  };
  std::array pqr{
      points.insert({0.0, -1.0, 1.0}),
      points.insert({3.0, -1.0, 1.0}),
      points.insert({0.0, -1.0, 4.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, NoIntersection) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 3.0, 0.0}),
  };
  std::array pqr{
      points.insert({2.0, 2.0, -1.0}),
      points.insert({5.0, 2.0, -1.0}),
      points.insert({2.0, 2.0, 2.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, EdgeEdge1) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 3.0, 0.0}),
  };
  std::array pqr{
      points.insert({2.0, 1.0, -1.0}),
      points.insert({5.0, 1.0, -1.0}),
      points.insert({2.0, 1.0, 2.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, EdgeFace) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 3.0, 0.0}),
  };
  std::array pqr{
      points.insert({1.0, 1.0, -1.0}),
      points.insert({4.0, 1.0, -1.0}),
      points.insert({1.0, 1.0, 2.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, EdgeEdge2) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 3.0, 0.0}),
  };
  std::array pqr{
      points.insert({0.0, 1.0, -1.0}),
      points.insert({3.0, 1.0, -1.0}),
      points.insert({0.0, 1.0, 2.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, VertexEdge) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 3.0, 0.0}),
  };
  std::array pqr{
      points.insert({0.0, 1.0, -3.0}),
      points.insert({3.0, 1.0, -3.0}),
      points.insert({0.0, 1.0, 0.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, VertexFace) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 3.0, 0.0}),
  };
  std::array pqr{
      points.insert({1.0, 1.0, -3.0}),
      points.insert({4.0, 1.0, -3.0}),
      points.insert({1.0, 1.0, 0.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, VertexVertex1) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 3.0, 0.0}),
  };
  std::array pqr{
      points.insert({0.0, 0.0, -3.0}),
      points.insert({3.0, 0.0, -3.0}),
      points.insert({0.0, 0.0, 0.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, VertexVertex2) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 3.0, 0.0}),
  };
  std::array pqr{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 0.0, 3.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, CoplanarNoIntersectionFast) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 3.0, 0.0}),
  };
  std::array pqr{
      points.insert({4.0, 4.0, 0.0}),
      points.insert({1.0, 4.0, 0.0}),
      points.insert({4.0, 1.0, 0.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, CoplanarNoIntersection) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 3.0, 0.0}),
  };
  std::array pqr{
      points.insert({5.0, 2.0, 0.0}),
      points.insert({2.0, 2.0, 0.0}),
      points.insert({5.0, -1.0, 0.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, CoplanarEdgeEdge) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({3.0, 0.0, 0.0}),
      points.insert({0.0, 3.0, 0.0}),
  };
  std::array pqr{
      points.insert({2.0, 2.0, 0.0}),
      points.insert({-1.0, 2.0, 0.0}),
      points.insert({2.0, -1.0, 0.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, CoplanarVertexFace) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({5.0, 0.0, 0.0}),
      points.insert({0.0, 5.0, 0.0}),
  };
  std::array pqr{
      points.insert({2.0, 2.0, 0.0}),
      points.insert({1.0, 2.0, 0.0}),
      points.insert({2.0, 1.0, 0.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, CoplanarVertexEdge) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({2.0, 0.0, 0.0}),
      points.insert({0.0, 2.0, 0.0}),
  };
  std::array pqr{
      points.insert({1.0, 1.0, 0.0}),
      points.insert({0.0, 1.0, 0.0}),
      points.insert({1.0, 0.0, 0.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, CoplanarVertexVertex) {
  Point_list points;
  std::array abc{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({1.0, 0.0, 0.0}),
      points.insert({0.0, 1.0, 0.0}),
  };
  std::array pqr{
      points.insert({0.0, 0.0, 0.0}),
      points.insert({1.0, 0.0, 0.0}),
      points.insert({0.0, 1.0, 0.0}),
  };

  ASSERT_TRUE(test(points, abc, pqr));
}

TEST(FaceFaceIntersectionTest, CubeCoplanar) {
  Point_list points;
  auto cube = make_cube(points, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0});

  for (const auto& left_face : cube) {
    for (const auto& right_face : cube) {
      ASSERT_TRUE(test(points, left_face, right_face));
    }
  }
}

TEST(FaceFaceIntersectionTest, CubeOpposite) {
  Point_list points;
  auto left_cube = make_cube(points, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0});
  auto right_cube = make_cube(points, {1.0, 1.0, 1.0}, {0.0, 0.0, 0.0});

  for (const auto& left_face : left_cube) {
    for (const auto& right_face : right_cube) {
      ASSERT_TRUE(test(points, left_face, right_face));
    }
  }
}

TEST(FaceFaceIntersectionTest, IntersectingCubes1) {
  Point_list points;
  auto left_cube = make_cube(points, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0});
  auto right_cube = make_cube(points, {0.5, 0.0, 0.0}, {1.5, 1.0, 1.0});

  for (const auto& left_face : left_cube) {
    for (const auto& right_face : right_cube) {
      ASSERT_TRUE(test(points, left_face, right_face));
    }
  }
}

TEST(FaceFaceIntersectionTest, IntersectingCubes2) {
  Point_list points;
  auto left_cube = make_cube(points, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0});
  auto right_cube = make_cube(points, {0.5, 0.5, 0.0}, {1.5, 1.5, 1.0});

  for (const auto& left_face : left_cube) {
    for (const auto& right_face : right_cube) {
      ASSERT_TRUE(test(points, left_face, right_face));
    }
  }
}

TEST(FaceFaceIntersectionTest, IntersectingCubes3) {
  Point_list points;
  auto left_cube = make_cube(points, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0});
  auto right_cube = make_cube(points, {0.5, 0.5, 0.5}, {1.5, 1.5, 1.5});

  for (const auto& left_face : left_cube) {
    for (const auto& right_face : right_cube) {
      ASSERT_TRUE(test(points, left_face, right_face));
    }
  }
}

TEST(FaceFaceIntersectionTest, DisjointCubes1) {
  Point_list points;
  auto left_cube = make_cube(points, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0});
  auto right_cube = make_cube(points, {1.5, 0.0, 0.0}, {2.5, 1.0, 1.0});

  for (const auto& left_face : left_cube) {
    for (const auto& right_face : right_cube) {
      ASSERT_TRUE(test(points, left_face, right_face));
    }
  }
}

TEST(FaceFaceIntersectionTest, DisjointCubes2) {
  Point_list points;
  auto left_cube = make_cube(points, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0});
  auto right_cube = make_cube(points, {1.5, 1.5, 0.0}, {2.5, 2.5, 1.0});

  for (const auto& left_face : left_cube) {
    for (const auto& right_face : right_cube) {
      ASSERT_TRUE(test(points, left_face, right_face));
    }
  }
}

TEST(FaceFaceIntersectionTest, DisjointCubes3) {
  Point_list points;
  auto left_cube = make_cube(points, {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0});
  auto right_cube = make_cube(points, {1.5, 1.5, 1.5}, {2.5, 2.5, 2.5});

  for (const auto& left_face : left_cube) {
    for (const auto& right_face : right_cube) {
      ASSERT_TRUE(test(points, left_face, right_face));
    }
  }
}
