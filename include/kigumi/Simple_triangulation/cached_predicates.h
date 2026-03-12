#pragma once

#include <CGAL/enum.h>
#include <kigumi/Point_list.h>
#include <kigumi/Simple_triangulation/Triangulation_traits.h>

#include <array>
#include <boost/container_hash/hash.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <utility>

namespace kigumi {

template <std::size_t N>
static CGAL::Sign sort_antisym(std::array<std::size_t, N>& ids) {
  auto parity = CGAL::POSITIVE;
  for (std::size_t i = 1; i < ids.size(); ++i) {
    auto j = i;
    while (j > 0 && ids.at(j) > ids.at(j - 1)) {
      std::swap(ids.at(j), ids.at(j - 1));
      parity = -parity;
      --j;
    }
  }
  return parity;
}

template <class K>
class Cached_compare_x {
  using Key = std::pair<std::size_t, std::size_t>;
  using Traits = Triangulation_traits<K>;
  using Point_list = Point_list<K>;

 public:
  explicit Cached_compare_x(const Traits* t, const Point_list* points) : t_(t), points_(points) {}

  int operator()(std::size_t i, std::size_t j) const {
    auto parity = 1;
    if (i > j) {
      std::swap(i, j);
      parity = -1;
    }
    auto [it, inserted] = cache_.emplace(Key{i, j}, 0);
    if (inserted) {
      it->second = t_->compare_x(points_->at(i), points_->at(j));
    }
    return parity * it->second;
  }

 private:
  const Traits* t_;
  const Point_list* points_;
  mutable boost::unordered_flat_map<Key, int, boost::hash<Key>> cache_;
};

template <class K>
class Cached_compare_y {
  using Key = std::pair<std::size_t, std::size_t>;
  using Traits = Triangulation_traits<K>;
  using Point_list = Point_list<K>;

 public:
  explicit Cached_compare_y(const Traits* t, const Point_list* points) : t_(t), points_(points) {}

  int operator()(std::size_t i, std::size_t j) const {
    auto parity = 1;
    if (i > j) {
      std::swap(i, j);
      parity = -1;
    }
    auto [it, inserted] = cache_.emplace(Key{i, j}, 0);
    if (inserted) {
      it->second = t_->compare_y(points_->at(i), points_->at(j));
    }
    return parity * it->second;
  }

 private:
  const Traits* t_;
  const Point_list* points_;
  mutable boost::unordered_flat_map<Key, int, boost::hash<Key>> cache_;
};

template <class K>
class Cached_orientation {
  using Key = std::array<std::size_t, 3>;
  using Traits = Triangulation_traits<K>;
  using Point_list = Point_list<K>;

 public:
  Cached_orientation(const Traits* t, const Point_list* points) : t_(t), points_(points) {}

  int operator()(std::size_t a, std::size_t b, std::size_t c) const {
    Key key{a, b, c};
    auto parity = sort_antisym(key);

    auto [it, inserted] = cache_.emplace(key, 0);
    if (inserted) {
      it->second = t_->orientation(points_->at(key[0]), points_->at(key[1]), points_->at(key[2]));
    }

    return parity * it->second;
  }

 private:
  const Traits* t_;
  const Point_list* points_;
  mutable boost::unordered_flat_map<Key, int, boost::hash<Key>> cache_;
};

template <class K>
class Cached_side_of_oriented_circle {
  using Key = std::array<std::size_t, 4>;
  using Traits = Triangulation_traits<K>;
  using Point_list = Point_list<K>;

 public:
  Cached_side_of_oriented_circle(const Traits* t, const Point_list* points)
      : t_(t), points_(points) {}

  int operator()(std::size_t a, std::size_t b, std::size_t c, std::size_t d) const {
    Key key{a, b, c, d};
    auto parity = sort_antisym(key);

    auto [it, inserted] = cache_.emplace(key, 0);
    if (inserted) {
      it->second = t_->side_of_oriented_circle(points_->at(key[0]), points_->at(key[1]),
                                               points_->at(key[2]), points_->at(key[3]));
    }

    return parity * it->second;
  }

 private:
  const Traits* t_;
  const Point_list* points_;
  mutable boost::unordered_flat_map<Key, int, boost::hash<Key>> cache_;
};

}  // namespace kigumi
