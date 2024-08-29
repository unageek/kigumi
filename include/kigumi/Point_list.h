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

  auto begin() const { return points_.begin(); }

  auto end() const { return points_.end(); }

  std::size_t insert(const Point& p) { return insert(Point{p}); }

  std::size_t insert(Point&& p) {
    if (!check_uniqueness_) {
      points_.push_back(std::move(p));
      return points_.size() - 1;
    }

    auto [it, inserted] = point_to_index_.emplace(p, points_.size());

    if (inserted) {
      points_.push_back(std::move(p));
    }

    return it->second;
  }

  std::vector<Point> take_points() { return std::move(points_); }

  void reserve(std::size_t capacity) {
    points_.reserve(capacity);
    if (!check_uniqueness_) {
      return;
    }

    point_to_index_.reserve(capacity);
  }

  std::size_t size() const { return points_.size(); }

  void start_uniqueness_check() { check_uniqueness_ = true; }

  void stop_uniqueness_check() {
    check_uniqueness_ = false;
    point_to_index_ = {};
  }

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
  bool check_uniqueness_{};
};

}  // namespace kigumi
