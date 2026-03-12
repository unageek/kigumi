#pragma once

#include <CGAL/Kernel/global_functions.h>
#include <kigumi/Mesh_indices.h>
#include <kigumi/Point_list.h>
#include <kigumi/Simple_triangulation/Triangulation_traits.h>
#include <kigumi/Simple_triangulation/delaunay.h>
#include <kigumi/Simple_triangulation/monotone.h>
#include <kigumi/Triangle_region.h>

#include <algorithm>
#include <array>
#include <stdexcept>
#include <utility>
#include <vector>

namespace kigumi {

template <class K>
class Triangulation_simple {
  using Point = typename K::Point_3;
  using Edge = std::pair<std::size_t, std::size_t>;
  using Face = std::array<std::size_t, 3>;
  using Traits = Triangulation_traits<K>;
  using Monotone = Monotone<K>;
  using Delaunay_refine = Delaunay_refine<K>;
  using Point_list = Point_list<K>;

 public:
  using Intersection_of_constraints_exception = typename std::runtime_error;
  using Vertex_handle = std::size_t;

  Triangulation_simple(const Point_list& points, Triangle_region, std::size_t a, std::size_t b,
                       std::size_t c)
      : points_{points}, traits_{make_traits(points_.at(a), points_.at(b), points_.at(c))} {
    insert(a, {});
    insert(b, {});
    insert(c, {});
  }

  void finalize() const {
    Monotone monotone{traits_};
    std::sort(vertices_.begin(), vertices_.end());
    vertices_.erase(std::unique(vertices_.begin(), vertices_.end()), vertices_.end());
    std::sort(edges_.begin(), edges_.end());
    edges_.erase(std::unique(edges_.begin(), edges_.end()), edges_.end());
    faces_ = monotone.monotone_triangulate(points_, vertices_, edges_);
    Delaunay_refine delaunay_refine{traits_};
    faces_ = delaunay_refine.delaunary_refine(points_, vertices_, edges_, faces_);
  }

  template <class OutputIterator>
  std::size_t get_faces(OutputIterator faces) const {
    std::size_t count{};
    for (auto [i, j, k] : faces_) {
      auto a = Vertex_index{i};
      auto b = Vertex_index{j};
      auto c = Vertex_index{k};
      *faces++ = {a, b, c};
      ++count;
    }
    return count;
  }

  Vertex_handle insert(std::size_t id, Triangle_region) {
    vertices_.push_back(id);
    return id;
  }

  void insert_constraint(std::size_t i, std::size_t j) {
    if (i > j) {
      std::swap(i, j);
    }
    edges_.emplace_back(i, j);
  }

  static Traits make_traits(const Point& pa, const Point& pb, const Point& pc) {
    return Traits{CGAL::normal(pa, pb, pc)};
  }

 private:
  const Point_list& points_;
  const Traits traits_;
  mutable std::vector<std::size_t> vertices_;
  mutable std::vector<Edge> edges_;
  mutable std::vector<Face> faces_;
};

}  // namespace kigumi
