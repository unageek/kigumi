#pragma once

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/polygon_soup_io.h>
#include <CGAL/gmpxx.h>
#include <kigumi/AABB_tree/AABB_leaf.h>
#include <kigumi/AABB_tree/AABB_tree.h>
#include <kigumi/Mesh_items.h>
#include <kigumi/Mesh_iterators.h>
#include <kigumi/io.h>

#include <array>
#include <boost/range/iterator_range.hpp>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace kigumi {

template <class K, class FaceData = std::nullptr_t>
class Polygon_soup {
  using Face_data = FaceData;
  using Point = typename K::Point_3;
  using Triangle = typename K::Triangle_3;

 public:
  class Leaf : public AABB_leaf {
   public:
    Leaf(const Triangle& tri, Face_handle fh) : AABB_leaf(tri.bbox()), fh_(fh) {}

    Face_handle face_handle() const { return fh_; }

   private:
    Face_handle fh_;
  };

  Polygon_soup() = default;

  ~Polygon_soup() = default;

  Polygon_soup(const Polygon_soup& other)
      : points_(other.points_), faces_(other.faces_), face_data_(other.face_data_) {}

  Polygon_soup(Polygon_soup&& other) noexcept
      : points_(std::move(other.points_)),
        faces_(std::move(other.faces_)),
        face_data_(std::move(other.face_data_)),
        aabb_tree_(std::move(other.aabb_tree_)) {}

  Polygon_soup& operator=(const Polygon_soup& other) {
    if (this != &other) {
      points_ = other.points_;
      faces_ = other.faces_;
      face_data_ = other.face_data_;
      aabb_tree_.reset();
    }
    return *this;
  }

  Polygon_soup& operator=(Polygon_soup&& other) noexcept {
    points_ = std::move(other.points_);
    faces_ = std::move(other.faces_);
    face_data_ = std::move(other.face_data_);
    aabb_tree_ = std::move(other.aabb_tree_);
    return *this;
  }

  explicit Polygon_soup(const std::string& filename) {
    // CGAL::IO::read_OBJ does not support std::vector<std::array<...>>.
    std::vector<std::vector<std::size_t>> faces;
    CGAL::IO::read_polygon_soup(filename, points_, faces);

    faces_.reserve(faces.size());
    for (const auto& face : faces) {
      if (face.size() != 3) {
        throw std::runtime_error("not a triangle mesh");
      }
      faces_.push_back({Vertex_handle{face[0]}, Vertex_handle{face[1]}, Vertex_handle{face[2]}});
      face_data_.push_back({});
    }
  }

  Polygon_soup(std::vector<Point>&& points, std::vector<Face>&& faces,
               std::vector<FaceData>&& face_data)
      : points_(std::move(points)), faces_(std::move(faces)), face_data_(std::move(face_data)) {}

  Vertex_handle add_vertex(const Point& p) {
    points_.push_back(p);
    return {points_.size() - 1};
  }

  Face_handle add_face(const Face& face) {
    faces_.push_back(face);
    face_data_.push_back({});
    return {faces_.size() - 1};
  }

  void save(const std::string& filename) {
    using Epick = CGAL::Exact_predicates_inexact_constructions_kernel;

    std::vector<Epick::Point_3> points;
    points.reserve(points_.size());
    for (const auto& p : points_) {
      points.emplace_back(CGAL::to_double(p.x()), CGAL::to_double(p.y()), CGAL::to_double(p.z()));
    }

    std::vector<std::array<std::size_t, 3>> faces;
    faces.reserve(faces_.size());
    for (const auto& f : faces_) {
      faces.push_back({f[0].i, f[1].i, f[2].i});
    }

    CGAL::IO::write_polygon_soup(filename, points, faces, CGAL::parameters::stream_precision(17));
  }

  std::size_t num_vertices() const { return points_.size(); }

  std::size_t num_faces() const { return faces_.size(); }

  Vertex_iterator vertices_begin() const { return Vertex_iterator({0}); }

  Vertex_iterator vertices_end() const { return Vertex_iterator({points_.size()}); }

  Face_iterator faces_begin() const { return Face_iterator({0}); }

  Face_iterator faces_end() const { return Face_iterator({faces_.size()}); }

  auto faces() const { return boost::make_iterator_range(faces_begin(), faces_end()); }

  Face_data& data(Face_handle handle) { return face_data_.at(handle.i); }

  const Face_data& data(Face_handle handle) const { return face_data_.at(handle.i); }

  const Face& face(Face_handle handle) const { return faces_.at(handle.i); }

  const Point& point(Vertex_handle handle) const { return points_.at(handle.i); }

  Triangle triangle(Face_handle handle) const {
    const auto& f = face(handle);
    return {point(f[0]), point(f[1]), point(f[2])};
  }

  const AABB_tree<Leaf>& aabb_tree() const {
    std::lock_guard<std::mutex> lk(aabb_tree_mutex_);

    if (!aabb_tree_) {
      std::vector<Leaf> leaves;
      for (auto fh : faces()) {
        leaves.emplace_back(triangle(fh), fh);
      }
      aabb_tree_ = std::make_unique<AABB_tree<Leaf>>(std::move(leaves));
    }

    return *aabb_tree_;
  }

 private:
  std::vector<Point> points_;
  std::vector<Face> faces_;
  std::vector<Face_data> face_data_;
  mutable std::unique_ptr<AABB_tree<Leaf>> aabb_tree_;
  mutable std::mutex aabb_tree_mutex_;
};

template <class K, class Face_data>
struct Write<Polygon_soup<K, Face_data>> {
  static void write(std::ostream& out, const Polygon_soup<K, Face_data>& tt) {
    do_write(out, tt.num_vertices());
    do_write(out, tt.num_faces());

    for (std::size_t i = 0; i < tt.num_vertices(); ++i) {
      const auto& p = tt.point({i});

      if (p.approx().x().is_point() && p.approx().y().is_point() && p.approx().z().is_point()) {
        do_write(out, false);
        do_write(out, p.approx().x().inf());
        do_write(out, p.approx().y().inf());
        do_write(out, p.approx().z().inf());
      } else {
        do_write(out, true);
        do_write(out, p.exact().x());
        do_write(out, p.exact().y());
        do_write(out, p.exact().z());
      }
    }

    for (const auto& fh : tt.faces()) {
      const auto& f = tt.face(fh);
      const auto& f_data = tt.data(fh);
      do_write(out, f[0]);
      do_write(out, f[1]);
      do_write(out, f[2]);
      do_write(out, f_data);
    }
  }
};

template <class K, class Face_data>
struct Read<Polygon_soup<K, Face_data>> {
  static void read(std::istream& in, Polygon_soup<K, Face_data>& tt) {
    std::size_t num_vertices{};
    std::size_t num_faces{};
    do_read(in, num_vertices);
    do_read(in, num_faces);

    for (std::size_t i = 0; i < num_vertices; ++i) {
      bool is_exact{};
      do_read(in, is_exact);

      if (!is_exact) {
        double x{};
        double y{};
        double z{};
        do_read(in, x);
        do_read(in, y);
        do_read(in, z);
        tt.add_vertex({x, y, z});
      } else {
        mpq_class x;
        mpq_class y;
        mpq_class z;
        do_read(in, x);
        do_read(in, y);
        do_read(in, z);
        tt.add_vertex({CGAL::Lazy_exact_nt<mpq_class>(x), CGAL::Lazy_exact_nt<mpq_class>(y),
                       CGAL::Lazy_exact_nt<mpq_class>(z)});
      }
    }

    for (std::size_t i = 0; i < num_faces; ++i) {
      Face face{};
      Face_data f_data{};
      do_read(in, face[0]);
      do_read(in, face[1]);
      do_read(in, face[2]);
      do_read(in, f_data);
      auto fh = tt.add_face(face);
      tt.data(fh) = f_data;
    }
  }
};

}  // namespace kigumi
