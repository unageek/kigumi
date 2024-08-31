#pragma once

#include <CGAL/enum.h>
#include <kigumi/Null_data.h>
#include <kigumi/Side_of_triangle_soup.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/io.h>

#include <cstdint>
#include <iostream>
#include <utility>

namespace kigumi {

enum class Region_kind : std::uint8_t {
  Empty,
  Full,
  Normal,
};

template <>
struct Write<Region_kind> {
  void operator()(std::ostream& out, const Region_kind& t) const {
    kigumi_write<std::uint8_t>(out, static_cast<std::uint8_t>(t));
  }
};

template <>
struct Read<Region_kind> {
  void operator()(std::istream& in, Region_kind& t) const {
    std::uint8_t x{};
    kigumi_read<std::uint8_t>(in, x);
    t = static_cast<Region_kind>(x);
  }
};

template <class K, class FaceData>
class Boolean_region_builder;

template <class K, class FaceData = Null_data>
class Region {
  using Boolean_region_builder = Boolean_region_builder<K, FaceData>;
  using Point = typename K::Point_3;
  using Side_of_triangle_soup = Side_of_triangle_soup<K, FaceData>;
  using Triangle_soup = Triangle_soup<K, FaceData>;

 public:
  Region() = default;

  explicit Region(Triangle_soup boundary)
      : kind_{Region_kind::Normal}, boundary_{std::move(boundary)} {
    if (boundary_.num_faces() == 0) {
      throw std::invalid_argument("region boundary must not be empty");
    }
  }

  const Triangle_soup& boundary() const { return boundary_; }

  // NOTE: CGAL::Oriented_side and CGAL::Bounded_side have opposite signs.
  CGAL::Bounded_side bounded_side(const Point& p) const {
    if (is_empty()) {
      return CGAL::ON_UNBOUNDED_SIDE;
    }
    if (is_full()) {
      return CGAL::ON_BOUNDED_SIDE;
    }
    switch (Side_of_triangle_soup{}(boundary_, p)) {
      case CGAL::ON_NEGATIVE_SIDE:
        return CGAL::ON_BOUNDED_SIDE;
      case CGAL::ON_POSITIVE_SIDE:
        return CGAL::ON_UNBOUNDED_SIDE;
      default:
        return CGAL::ON_BOUNDARY;
    }
  }

  static Region empty() { return Region{Region_kind::Empty}; }

  static Region full() { return Region{Region_kind::Full}; }

  bool is_empty() const { return kind_ == Region_kind::Empty; }

  bool is_empty_or_full() const { return is_empty() || is_full(); }

  bool is_full() const { return kind_ == Region_kind::Full; }

 private:
  friend Boolean_region_builder;
  friend Read<Region>;
  friend Write<Region>;

  explicit Region(Region_kind kind) : kind_{kind} {}

  Region_kind kind_{Region_kind::Empty};
  Triangle_soup boundary_;
};

template <class K, class FaceData>
struct Write<Region<K, FaceData>> {
  void operator()(std::ostream& out, const Region<K, FaceData>& t) const {
    kigumi_write<Region_kind>(out, t.kind_);
    kigumi_write<Triangle_soup<K, FaceData>>(out, t.boundary_);
  }
};

template <class K, class FaceData>
struct Read<Region<K, FaceData>> {
  void operator()(std::istream& in, Region<K, FaceData>& t) const {
    kigumi_read<Region_kind>(in, t.kind_);
    kigumi_read<Triangle_soup<K, FaceData>>(in, t.boundary_);
  }
};

}  // namespace kigumi
