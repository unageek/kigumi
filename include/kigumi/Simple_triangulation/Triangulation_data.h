#pragma once

#include <kigumi/Simple_triangulation/utils.h>

#include <algorithm>
#include <array>
#include <boost/unordered/unordered_flat_map.hpp>
#include <utility>
#include <vector>

namespace kigumi {

template <class K>
class Triangulation_data {
  using Edge = std::pair<std::size_t, std::size_t>;
  using Face = std::array<std::size_t, 3>;
  using Star = std::vector<Edge>;

 public:
  Triangulation_data(std::vector<std::size_t> vertices, std::vector<Edge> edges)
      : edges_(std::move(edges)) {
    for (auto i : vertices) {
      stars_.emplace(i, Star{});
    }
    std::sort(edges_.begin(), edges_.end());
  }

  bool is_constraint(Edge edge) const {
    if (edge.first > edge.second) {
      std::swap(edge.first, edge.second);
    }
    auto it = bsearch_eq(edges_.begin(), edges_.end(), edge);
    return it != edges_.end();
  }

  void remove_triangle(Face face) {
    auto remove = [](Star& star, Edge edge) {
      auto it = std::find(star.begin(), star.end(), edge);
      if (it != star.end()) {
        std::swap(*it, star.back());
        star.pop_back();
      }
    };

    auto [i, j, k] = face;
    remove(stars_.at(i), {j, k});
    remove(stars_.at(j), {k, i});
    remove(stars_.at(k), {i, j});
  }

  void add_triangle(Face face) {
    auto [i, j, k] = face;
    stars_.at(i).emplace_back(j, k);
    stars_.at(j).emplace_back(k, i);
    stars_.at(k).emplace_back(i, j);
  }

  std::size_t opposite(Edge edge) const {
    auto [i, j] = edge;
    const auto& star = stars_.at(i);
    auto it = std::find_if(star.begin(), star.end(), [&](const Edge& e) { return e.first == j; });
    return it != star.end() ? it->second : -1;
  }

  void flip(Edge edge) {
    auto [i, j] = edge;
    auto a = opposite({i, j});
    auto b = opposite({j, i});
    remove_triangle({i, j, a});
    remove_triangle({j, i, b});
    add_triangle({i, b, a});
    add_triangle({j, a, b});
  }

  std::vector<Face> faces() const {
    std::vector<Face> faces;
    for (const auto& [i, star] : stars_) {
      for (const auto& [j, k] : star) {
        if (i < j && i < k) {
          faces.push_back({i, j, k});
        }
      }
    }
    return faces;
  }

  const boost::unordered_flat_map<std::size_t, Star>& stars() const { return stars_; }

  const std::vector<Edge>& edges() const { return edges_; }

 private:
  boost::unordered_flat_map<std::size_t, Star> stars_;
  std::vector<Edge> edges_;
};

}  // namespace kigumi
