#pragma once

#include <CGAL/enum.h>
#include <kigumi/Point_list.h>
#include <kigumi/Simple_triangulation/Triangulation_traits.h>
#include <kigumi/Simple_triangulation/cached_predicates.h>
#include <kigumi/Simple_triangulation/utils.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>
#include <utility>
#include <vector>

namespace kigumi {

template <class K>
class Monotone {
  using Traits = Triangulation_traits<K>;
  using Point = typename Traits::Point;
  using Edge = std::pair<std::size_t, std::size_t>;
  using Face = std::array<std::size_t, 3>;
  using Point_list = Point_list<K>;
  using Compare_x = Cached_compare_x<K>;
  using Compare_y = Cached_compare_y<K>;
  using Orientation = Cached_orientation<K>;
  static constexpr std::size_t kInvalid{static_cast<std::size_t>(-1)};

 public:
  explicit Monotone(const Traits& t) : t_{&t} {}

  std::vector<Face> monotone_triangulate(const Point_list& points,
                                         const std::vector<std::size_t>& vertices,
                                         const std::vector<Edge>& edges) {
    Compare_x compare_x{t_, &points};
    Compare_y compare_y{t_, &points};
    Orientation orientation{t_, &points};
    Event_less event_less{t_, &points, &compare_x, &compare_y, &orientation};
    Find_split find_split{t_, &points, &orientation};
    Test_point test_point{t_, &points, &orientation};

    std::vector<Event> events;
    for (auto a : vertices) {
      events.emplace_back(a, kInvalid, Event_type::kPoint);
    }
    for (auto [a, b] : edges) {
      auto d = compare_x(a, b);
      if (d < 0) {
        events.emplace_back(a, b, Event_type::kStart);
        events.emplace_back(b, a, Event_type::kEnd);
      } else if (d > 0) {
        events.emplace_back(b, a, Event_type::kStart);
        events.emplace_back(a, b, Event_type::kEnd);
      }
    }
    std::sort(events.begin(), events.end(), event_less);

    std::vector<Partial_hull> hulls;
    hulls.push_back({kInvalid, kInvalid, {}, {}});

    std::vector<Face> faces;
    for (const auto& event : events) {
      switch (event.type) {
        case Event_type::kPoint:
          add_point(faces, hulls, event, test_point, &orientation);
          break;
        case Event_type::kStart:
          split_hulls(hulls, event, find_split);
          break;
        case Event_type::kEnd:
          merge_hulls(hulls, event);
          break;
      }
    }

    return faces;
  }

 private:
  enum class Event_type { kPoint = 0, kEnd = 1, kStart = 2 };

  struct Event {
    std::size_t a;
    std::size_t b;
    Event_type type;
  };

  struct Partial_hull {
    std::size_t a;
    std::size_t b;
    std::vector<std::size_t> lower_ids;
    std::vector<std::size_t> upper_ids;
  };

  struct Event_less {
   public:
    explicit Event_less(const Traits* t, const Point_list* points, const Compare_x* compare_x,
                        const Compare_y* compare_y, const Orientation* orientation)
        : t_(t),
          points_(points),
          compare_x_(compare_x),
          compare_y_(compare_y),
          orientation_(orientation) {}

    bool operator()(const Event& a, const Event& b) const {
      if (a.a != b.a) {
        auto d = (*compare_x_)(a.a, b.a);
        if (d != 0) {
          return d < 0;
        }

        d = (*compare_y_)(a.a, b.a);
        if (d != 0) {
          return d < 0;
        }
      }

      if (a.type != b.type) {
        return a.type < b.type;
      }

      assert(a.b != b.b);
      assert(a.type != Event_type::kPoint && b.type != Event_type::kPoint);

      auto d = (*orientation_)(a.a, a.b, b.b);
      assert(d != 0);
      return d > 0;
    }

   private:
    const Traits* t_;
    const Point_list* points_;
    const Compare_x* compare_x_;
    const Compare_y* compare_y_;
    const Orientation* orientation_;
  };

  class Find_split {
   public:
    explicit Find_split(const Traits* t, const Point_list* points, const Orientation* orientation)
        : t_(t), points_(points), orientation_(orientation) {}

    bool operator()(const Partial_hull& hull, const Event& event) const {
      return compare(hull, event) < 0;
    }

    bool operator()(const Event& event, const Partial_hull& hull) const {
      return compare(hull, event) > 0;
    }

   private:
    int compare(const Partial_hull& hull, const Event& event) const {
      if (hull.a == kInvalid) {
        // Sentinel hull
        return -1;
      }

      auto d = hull.a != event.a ? (*orientation_)(hull.a, hull.b, event.a)
                                 : (*orientation_)(hull.a, hull.b, event.b);
      assert(d != 0);
      return -d;
    }

    const Traits* t_;
    const Point_list* points_;
    const Orientation* orientation_;
  };

  class Test_point {
   public:
    explicit Test_point(const Traits* t, const Point_list* points, const Orientation* orientation)
        : t_(t), points_(points), orientation_(orientation) {}

    bool operator()(const Partial_hull& hull, std::size_t point) const {
      return compare(hull, point) < 0;
    }

    bool operator()(std::size_t point, const Partial_hull& hull) const {
      return compare(hull, point) > 0;
    }

   private:
    int compare(const Partial_hull& hull, std::size_t point) const {
      if (hull.a == kInvalid) {
        // Sentinel hull
        return -1;
      }

      if (hull.a == point || hull.b == point) {
        return 0;
      }

      auto d = (*orientation_)(hull.a, hull.b, point);
      return -d;
    }

    const Traits* t_;
    const Point_list* points_;
    const Orientation* orientation_;
  };

  void add_point(std::vector<Face>& faces, std::vector<Partial_hull>& hulls, const Event& event,
                 const Test_point& test_point, const Orientation* orientation) {
    auto [lo, hi] = bsearch_eq_range(hulls.begin(), hulls.end(), event.a, test_point);
    lo = std::prev(lo);

    // lo: the last hull whose edge sees the new point above it.
    // hi: the first hull whose edge sees the new point below it.

    for (auto it = lo; it != hi; ++it) {
      auto& hull = *it;

      auto& lower = hull.lower_ids;
      auto m = lower.size();
      while (m > 1 && (*orientation)(lower.at(m - 2), lower.at(m - 1), event.a) < 0) {
        // After inserting the new point `R`, the lower chain becomes `... -> P -> Q -> R`.
        // If `P -> Q -> R` is CW, `Q` cannot remain on the lower convex chain,
        // so remove it and emit the triangle `PQR`.
        faces.push_back({lower.at(m - 1), lower.at(m - 2), event.a});
        --m;
      }
      lower.resize(m);
      lower.push_back(event.a);

      auto& upper = hull.upper_ids;
      m = upper.size();
      while (m > 1 && (*orientation)(upper.at(m - 2), upper.at(m - 1), event.a) > 0) {
        // After inserting the new point `R`, the upper chain becones `... -> P -> Q -> R`.
        // If `P -> Q -> R` is CCW, `Q` cannot remain on the upper convex chain,
        // so remove it and emit the triangle `PQR`.
        faces.push_back({upper.at(m - 2), upper.at(m - 1), event.a});
        --m;
      }
      upper.resize(m);
      upper.push_back(event.a);
    }
  }

  void split_hulls(std::vector<Partial_hull>& hulls, const Event& event,
                   const Find_split& find_split) {
    auto it = bsearch_le(hulls.begin(), hulls.end(), event, find_split);
    auto& hull = *it;
    auto upper_ids = std::move(hull.upper_ids);
    auto x = upper_ids.back();
    hull.upper_ids = {x};
    hulls.insert(std::next(it), {event.a, event.b, {x}, upper_ids});
  }

  void merge_hulls(std::vector<Partial_hull>& hulls, const Event& event) {
    auto it = std::find_if(hulls.begin(), hulls.end(),
                           [&](const auto& h) { return h.a == event.b && h.b == event.a; });
    auto& upper = *it;
    auto& lower = *std::prev(it);
    lower.upper_ids = std::move(upper.upper_ids);
    hulls.erase(it);
  }

  const Traits* t_;
};

}  // namespace kigumi
