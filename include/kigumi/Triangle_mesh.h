#pragma once

#include <CGAL/Bbox_3.h>
#include <kigumi/Mesh_entities.h>
#include <kigumi/Mesh_handles.h>
#include <kigumi/Mesh_iterators.h>
#include <kigumi/Null_data.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/parallel_sort.h>

#include <boost/range/iterator_range.hpp>
#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

namespace kigumi {

// Example 1:
//
//      3       2     indices_   face_indices_
//       +-----+            |        |
//      / \ b / \           0:  0 -> a:  (0, 1, 2)
//     / c \ / a \                   b:  (0, 2, 3)
//    +-----+-----+                  c:  (0, 3, 4)
//   4      0      1        3:  1 -> a
//                          4:  2 -> a
//                                   b
//                          6:  3 -> b
//                                   c
//                          8:  4 -> c
//                          9:  END
//
// Example 2:
//
//       5            indices_   face_indices_
//       +                  |        |
//      / \   + 4           0:  0 -> a:  (0, 1, 2)
//     / a \                1:  1 -> a
//    +-----+    + 3        2
//   0       1              2
//                          2
//       + 2                2:  5 -> a
//                          3:  END

template <class K, class FaceData = Null_data>
class Triangle_mesh {
  using Bbox = CGAL::Bbox_3;
  using Face_data = FaceData;
  using Point = typename K::Point_3;
  using Triangle = typename K::Triangle_3;

 public:
  Triangle_mesh() = default;

  ~Triangle_mesh() = default;

  Triangle_mesh(const Triangle_mesh& other)
      : points_{other.points_},
        faces_{other.faces_},
        face_data_{other.face_data_},
        indices_{other.indices_},
        face_indices_{other.face_indices_} {}

  Triangle_mesh(Triangle_mesh&& other) noexcept
      : points_{std::move(other.points_)},
        faces_{std::move(other.faces_)},
        face_data_{std::move(other.face_data_)},
        indices_{std::move(other.indices_)},
        face_indices_{std::move(other.face_indices_)} {}

  Triangle_mesh& operator=(const Triangle_mesh& other) {
    if (this != &other) {
      points_ = other.points_;
      faces_ = other.faces_;
      face_data_ = other.face_data_;
      indices_ = other.indices_;
      face_indices_ = other.face_indices_;
    }
    return *this;
  }

  Triangle_mesh& operator=(Triangle_mesh&& other) noexcept {
    points_ = std::move(other.points_);
    faces_ = std::move(other.faces_);
    face_data_ = std::move(other.face_data_);
    indices_ = std::move(other.indices_);
    face_indices_ = std::move(other.face_indices_);
    return *this;
  }

  explicit Triangle_mesh(std::vector<Point> points) : points_{std::move(points)} {}

  Vertex_handle add_vertex(const Point& p) {
    points_.push_back(p);
    return {points_.size() - 1};
  }

  Face_handle add_face(const Face& face) {
    faces_.push_back(face);
    face_data_.emplace_back();
    return {faces_.size() - 1};
  }

  void finalize() {
    std::vector<std::pair<Vertex_handle, Face_handle>> map;
    std::size_t face_index{};
    for (const auto& face : faces_) {
      Face_handle fh{face_index};
      map.emplace_back(face[0], fh);
      map.emplace_back(face[1], fh);
      map.emplace_back(face[2], fh);
      ++face_index;
    }

    parallel_sort(map.begin(), map.end());

    std::size_t index{};
    std::ptrdiff_t prev_vi{-1};
    for (const auto& [vh, fh] : map) {
      face_indices_.push_back(fh);

      auto vi = static_cast<std::ptrdiff_t>(vh.i);
      for (std::ptrdiff_t i = 0; i < vi - prev_vi; ++i) {
        indices_.push_back(index);
      }
      prev_vi = vi;
      ++index;
    }
    indices_.push_back(index);
  }

  std::size_t num_vertices() const { return points_.size(); }

  std::size_t num_faces() const { return faces_.size(); }

  Vertex_iterator vertices_begin() const { return Vertex_iterator({0}); }

  Vertex_iterator vertices_end() const { return Vertex_iterator({points_.size()}); }

  auto vertices() const { return boost::make_iterator_range(vertices_begin(), vertices_end()); }

  Face_iterator faces_begin() const { return Face_iterator({0}); }

  Face_iterator faces_end() const { return Face_iterator({faces_.size()}); }

  auto faces() const { return boost::make_iterator_range(faces_begin(), faces_end()); }

  auto faces_around_edge(const Edge& edge) const {
    auto i_it = face_indices_.begin() + indices_.at(edge[0].i);
    auto i_end = face_indices_.begin() + indices_.at(edge[0].i + 1);
    auto j_it = face_indices_.begin() + indices_.at(edge[1].i);
    auto j_end = face_indices_.begin() + indices_.at(edge[1].i + 1);
    return boost::make_iterator_range(Face_around_edge_iterator(i_it, i_end, j_it, j_end),
                                      Face_around_edge_iterator(i_end, i_end, j_end, j_end));
  }

  auto faces_around_face(Face_handle fh, const std::unordered_set<Edge>& border_edges) const {
    const auto& f = face(fh);
    auto e1 = make_edge(f[0], f[1]);
    auto e2 = make_edge(f[1], f[2]);
    auto e3 = make_edge(f[2], f[0]);
    auto end1 = faces_around_edge(e1).end();
    auto it1 = border_edges.contains(e1) ? end1 : faces_around_edge(e1).begin();
    auto end2 = faces_around_edge(e2).end();
    auto it2 = border_edges.contains(e2) ? end2 : faces_around_edge(e2).begin();
    auto end3 = faces_around_edge(e3).end();
    auto it3 = border_edges.contains(e3) ? end3 : faces_around_edge(e3).begin();
    return boost::make_iterator_range(
        Face_around_face_iterator(fh, it1, end1, it2, end2, it3, end3),
        Face_around_face_iterator(fh, end1, end1, end2, end2, end3, end3));
  }

  Face_data& data(Face_handle handle) { return face_data_.at(handle.i); }

  const Face_data& data(Face_handle handle) const { return face_data_.at(handle.i); }

  const Face& face(Face_handle handle) const { return faces_.at(handle.i); }

  const Point& point(Vertex_handle handle) const { return points_.at(handle.i); }

  Triangle triangle(Face_handle handle) const {
    const auto& f = face(handle);
    return {point(f[0]), point(f[1]), point(f[2])};
  }

  Bbox bbox() const { return CGAL::bbox_3(points_.begin(), points_.end()); }

  Triangle_soup<K, FaceData> into_Triangle_soup() {
    return {std::move(points_), std::move(faces_), std::move(face_data_)};
  }

 private:
  std::vector<Point> points_;
  std::vector<Face> faces_;
  std::vector<Face_data> face_data_;
  std::vector<std::size_t> indices_;
  std::vector<Face_handle> face_indices_;
};

}  // namespace kigumi
