#pragma once

#include <kigumi/AABB_tree/AABB_leaf.h>
#include <kigumi/AABB_tree/AABB_tree.h>
#include <kigumi/Mesh_items.h>
#include <kigumi/Mesh_iterators.h>
#include <kigumi/Null_data.h>
#include <kigumi/Point_list.h>
#include <kigumi/Polygon_soup.h>

#include <algorithm>
#include <boost/range/iterator_range.hpp>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <utility>
#include <vector>

namespace kigumi {

//  indices_   face_indices_
//        |        |
//        0:  0 -> a:  (0, 1, 2)
//                 b:  (0, 2, 3)
//                 c:  (0, 3, 4)
//        3:  1 -> a
//        4:  2 -> a
//                 b
//        6:  3 -> b
//                 c
//        8:  4 -> c
//        9:  end

template <class K, class FaceData = Null_data>
class Mesh {
  using Face_data = FaceData;
  using Point = typename K::Point_3;
  using Triangle = typename K::Triangle_3;

 public:
  class Leaf : public AABB_leaf {
   public:
    Leaf(const Triangle& tri, Face_handle fh) : AABB_leaf{tri.bbox()}, fh_{fh} {}

    Face_handle face_handle() const { return fh_; }

   private:
    Face_handle fh_;
  };

  Mesh() = default;

  ~Mesh() = default;

  Mesh(const Mesh& other)
      : point_list_{other.point_list_},
        points_{other.points_},
        faces_{other.faces_},
        face_data_{other.face_data_},
        indices_{other.indices_},
        face_indices_{other.face_indices_} {}

  Mesh(Mesh&& other) noexcept
      : point_list_{std::move(other.point_list_)},
        points_{std::move(other.points_)},
        faces_{std::move(other.faces_)},
        face_data_{std::move(other.face_data_)},
        indices_{std::move(other.indices_)},
        face_indices_{std::move(other.face_indices_)},
        aabb_tree_{std::move(other.aabb_tree_)} {}

  Mesh& operator=(const Mesh& other) {
    if (this != &other) {
      point_list_ = other.point_list_;
      points_ = other.points_;
      faces_ = other.faces_;
      face_data_ = other.face_data_;
      indices_ = other.indices_;
      face_indices_ = other.face_indices_;
      aabb_tree_.reset();
    }
    return *this;
  }

  Mesh& operator=(Mesh&& other) noexcept {
    point_list_ = std::move(other.point_list_);
    points_ = std::move(other.points_);
    faces_ = std::move(other.faces_);
    face_data_ = std::move(other.face_data_);
    indices_ = std::move(other.indices_);
    face_indices_ = std::move(other.face_indices_);
    aabb_tree_ = std::move(other.aabb_tree_);
    return *this;
  }

  Vertex_handle add_vertex(const Point& p) { return {point_list_.insert(p)}; }

  Face_handle add_face(const Face& face) {
    faces_.push_back(face);
    face_data_.emplace_back();
    return {faces_.size() - 1};
  }

  void finalize() {
    points_ = point_list_.into_vector();

    std::vector<std::pair<Vertex_handle, Face_handle>> map;
    std::size_t face_index{};
    for (const auto& face : faces_) {
      Face_handle fh{face_index};
      map.emplace_back(face[0], fh);
      map.emplace_back(face[1], fh);
      map.emplace_back(face[2], fh);
      ++face_index;
    }

    std::sort(map.begin(), map.end());

    std::size_t index{};
    Vertex_handle prev_vh;
    for (const auto& [vh, fh] : map) {
      face_indices_.push_back(fh);
      if (index == 0 || vh != prev_vh) {
        indices_.push_back(index);
      }
      prev_vh = vh;
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

  auto faces_around_face(Face_handle fh, const std::unordered_set<Edge>& border) {
    const auto& f = face(fh);
    auto e1 = make_edge(f[0], f[1]);
    auto e2 = make_edge(f[1], f[2]);
    auto e3 = make_edge(f[2], f[0]);
    auto end1 = faces_around_edge(e1).end();
    auto it1 = border.contains(e1) ? end1 : faces_around_edge(e1).begin();
    auto end2 = faces_around_edge(e2).end();
    auto it2 = border.contains(e2) ? end2 : faces_around_edge(e2).begin();
    auto end3 = faces_around_edge(e3).end();
    auto it3 = border.contains(e3) ? end3 : faces_around_edge(e3).begin();
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

  const AABB_tree<Leaf>& aabb_tree() const {
    std::lock_guard<std::mutex> lk{aabb_tree_mutex_};

    if (!aabb_tree_) {
      std::vector<Leaf> leaves;
      for (auto fh : faces()) {
        leaves.emplace_back(triangle(fh), fh);
      }
      aabb_tree_ = std::make_unique<AABB_tree<Leaf>>(std::move(leaves));
    }

    return *aabb_tree_;
  }

  Polygon_soup<K, FaceData> into_polygon_soup() {
    return {std::move(points_), std::move(faces_), std::move(face_data_)};
  }

 private:
  Point_list<K> point_list_;
  std::vector<Point> points_;
  std::vector<Face> faces_;
  std::vector<Face_data> face_data_;
  std::vector<std::size_t> indices_;
  std::vector<Face_handle> face_indices_;
  mutable std::unique_ptr<AABB_tree<Leaf>> aabb_tree_;
  mutable std::mutex aabb_tree_mutex_;
};

}  // namespace kigumi
