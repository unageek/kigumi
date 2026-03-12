#pragma once

#include <CGAL/enum.h>
#include <kigumi/Point_list.h>
#include <kigumi/Simple_triangulation/Triangulation_data.h>
#include <kigumi/Simple_triangulation/Triangulation_traits.h>
#include <kigumi/Simple_triangulation/cached_predicates.h>

#include <array>
#include <cassert>
#include <stack>
#include <utility>
#include <vector>

namespace kigumi {

template <class K>
class Delaunay_refine {
  using Edge = std::pair<std::size_t, std::size_t>;
  using Face = std::array<std::size_t, 3>;
  using Star = std::vector<Edge>;
  using Triangulation_data = Triangulation_data<K>;
  using Traits = Triangulation_traits<K>;
  using Point = typename Traits::Point;
  using Point_list = Point_list<K>;
  using Side_of_oriented_circle = Cached_side_of_oriented_circle<K>;
  static constexpr std::size_t kInvalid{static_cast<std::size_t>(-1)};

 public:
  explicit Delaunay_refine(const Traits& t) : t_{&t} {}

  std::vector<Face> delaunary_refine(const Point_list& points,
                                     const std::vector<std::size_t>& vertices,
                                     const std::vector<Edge>& edges,
                                     const std::vector<Face>& faces) {
    Side_of_oriented_circle side_of_oriented_circle{t_, &points};

    Triangulation_data td{vertices, edges};
    for (auto face : faces) {
      td.add_triangle(face);
    }

    const auto& stars = td.stars();
    std::stack<Edge> stack;

    for (auto a : vertices) {
      auto& star = stars.at(a);
      for (auto [b, x] : star) {
        // If order is not consistent, then skip edge
        if (b < a) {
          continue;
        }

        if (td.is_constraint({a, b})) {
          continue;
        }

        // Find opposite edge
        auto y = td.opposite({b, a});

        // If this is a boundary edge, don't flip it
        if (y == kInvalid) {
          continue;
        }

        // If edge is in circle, flip it
        if (side_of_oriented_circle(a, b, x, y) > 0) {
          stack.emplace(a, b);
        }
      }
    }

    while (!stack.empty()) {
      auto [a, b] = stack.top();
      stack.pop();

      auto x = td.opposite({a, b});
      if (x == kInvalid) {
        continue;
      }

      auto y = td.opposite({b, a});
      if (y == kInvalid) {
        continue;
      }

      assert(t_->orientation(points.at(a), points.at(b), points.at(x)) > 0);
      assert(t_->orientation(points.at(b), points.at(a), points.at(y)) > 0);

      // If edge is now delaunay, then don't flip it
      if (side_of_oriented_circle(a, b, x, y) <= 0) {
        continue;
      }

      // Flip the edge
      td.flip({a, b});

      // Test flipping neighboring edges
      test_flip(td, stack, {x, a}, y, side_of_oriented_circle);
      test_flip(td, stack, {a, y}, x, side_of_oriented_circle);
      test_flip(td, stack, {y, b}, x, side_of_oriented_circle);
      test_flip(td, stack, {b, x}, y, side_of_oriented_circle);
    }

    return td.faces();
  }

 private:
  void test_flip(const Triangulation_data& td, std::stack<Edge>& stack, Edge ab, std::size_t x,
                 const Side_of_oriented_circle& side_of_oriented_circle) {
    auto [a, b] = ab;

    auto y = td.opposite({b, a});
    if (y == kInvalid) {
      return;
    }

    // Swap edge if order flipped
    if (b < a) {
      std::swap(a, b);
      std::swap(x, y);
    }

    // Test if edge is constrained
    if (td.is_constraint({a, b})) {
      return;
    }

    assert(t_->orientation(points.at(a), points.at(b), points.at(x)) > 0);
    assert(t_->orientation(points.at(b), points.at(a), points.at(y)) > 0);

    // Test if edge is delaunay
    if (side_of_oriented_circle(a, b, x, y) > 0) {
      stack.emplace(a, b);
    }
  }

  const Traits* t_;
};

}  // namespace kigumi
