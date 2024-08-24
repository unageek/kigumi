#pragma once

#include <boost/container_hash/hash.hpp>
#include <unordered_map>
#include <utility>
#include <vector>

namespace kigumi {

template <class K>
class Point_list {
  using Point = typename K::Point_3;

 public:
  const Point& at(std::size_t i) const { return points_.at(i); }

  std::size_t insert(const Point& p) { return insert(Point{p}); }

  std::size_t insert(Point&& p) {
    auto it = point_to_index_.find(p);
    if (it != point_to_index_.end()) {
      return it->second;
    }

    auto i = points_.size();
    points_.push_back(p);
    point_to_index_.emplace(std::move(p), i);
    return i;
  }

  std::vector<Point> take_points() { return std::move(points_); }

  void reserve(std::size_t capacity) {
    points_.reserve(capacity);
    point_to_index_.reserve(capacity);
  }

  std::size_t size() const { return points_.size(); }

 private:
  struct Point_hash {
    std::size_t operator()(const Point& p) const noexcept {
      if (!p.approx().x().is_point() || !p.approx().y().is_point() || !p.approx().z().is_point()) {
        p.exact();
      }
      std::size_t seed{};
      boost::hash_combine(seed, p.approx().x().inf());
      boost::hash_combine(seed, p.approx().y().inf());
      boost::hash_combine(seed, p.approx().z().inf());
      return seed;
    }
  };

  std::vector<Point> points_;
  std::unordered_map<Point, std::size_t, Point_hash> point_to_index_;
};

}  // namespace kigumi
