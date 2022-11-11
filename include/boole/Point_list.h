#pragma once

#include <boost/container_hash/hash.hpp>
#include <functional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace boole {

template <class Kernel>
class Point_list {
  using Point = typename Kernel::Point_3;

 public:
  const Point& at(std::size_t i) const { return points_.at(i); }

  std::size_t insert(const Point& p) {
    auto it = point_to_index_.find(p);
    if (it != point_to_index_.end()) {
      return it->second;
    }

    auto i = points_.size();
    points_.push_back(p);
    point_to_index_.emplace(p, i);
    return i;
  }

  std::vector<Point> into_vector() const { return std::move(points_); }

 private:
  struct Point_hash {
    std::size_t operator()(const Point& p) const noexcept {
      std::size_t seed{};
      boost::hash_combine(seed, std::hash<double>()(CGAL::to_double(p.x().exact())));
      boost::hash_combine(seed, std::hash<double>()(CGAL::to_double(p.y().exact())));
      boost::hash_combine(seed, std::hash<double>()(CGAL::to_double(p.z().exact())));
      return seed;
    }
  };

  std::vector<Point> points_;
  std::unordered_map<Point, std::size_t, Point_hash> point_to_index_;
};

}  // namespace boole
