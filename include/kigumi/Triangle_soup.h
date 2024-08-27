#pragma once

#include <CGAL/Bbox_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_rational.h>
#include <CGAL/IO/polygon_soup_io.h>
#include <CGAL/Lazy_exact_nt.h>
#include <CGAL/number_utils.h>
#include <kigumi/AABB_tree/AABB_leaf.h>
#include <kigumi/AABB_tree/AABB_tree.h>
#include <kigumi/Mesh_entities.h>
#include <kigumi/Mesh_handles.h>
#include <kigumi/Mesh_iterators.h>
#include <kigumi/Null_data.h>
#include <kigumi/io.h>
#include <kigumi/mesh_utility.h>

#include <array>
#include <boost/range/iterator_range.hpp>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace kigumi {

template <class K, class FaceData = Null_data>
class Triangle_soup {
  using Bbox = CGAL::Bbox_3;
  using Face_data = FaceData;
  using Point = typename K::Point_3;
  using Triangle = typename K::Triangle_3;

 public:
  class Leaf : public AABB_leaf {
    using Bbox = CGAL::Bbox_3;

   public:
    Leaf(const Bbox& bbox, Face_handle fh) : AABB_leaf{bbox}, fh_{fh} {}

    Face_handle face_handle() const { return fh_; }

   private:
    Face_handle fh_;
  };

  Triangle_soup() = default;

  ~Triangle_soup() = default;

  Triangle_soup(const Triangle_soup& other)
      : points_{other.points_}, faces_{other.faces_}, face_data_{other.face_data_} {}

  Triangle_soup(Triangle_soup&& other) noexcept
      : points_{std::move(other.points_)},
        faces_{std::move(other.faces_)},
        face_data_{std::move(other.face_data_)},
        aabb_tree_{std::move(other.aabb_tree_)} {}

  Triangle_soup& operator=(const Triangle_soup& other) {
    if (this != &other) {
      points_ = other.points_;
      faces_ = other.faces_;
      face_data_ = other.face_data_;
      aabb_tree_.reset();
    }
    return *this;
  }

  Triangle_soup& operator=(Triangle_soup&& other) noexcept {
    points_ = std::move(other.points_);
    faces_ = std::move(other.faces_);
    face_data_ = std::move(other.face_data_);
    aabb_tree_ = std::move(other.aabb_tree_);
    return *this;
  }

  explicit Triangle_soup(const std::string& filename) {
    std::vector<std::vector<std::size_t>> faces;
    CGAL::IO::read_polygon_soup(filename, points_, faces);

    faces_.reserve(faces.size());
    face_data_.reserve(faces.size());

    for (const auto& f : faces) {
      if (f.size() < 3) {
        continue;
      }

      for (std::size_t i = 0; i < f.size() - 2; ++i) {
        faces_.push_back({Vertex_handle{f[0]}, Vertex_handle{f[i + 1]}, Vertex_handle{f[i + 2]}});
        face_data_.emplace_back();
      }
    }

    faces_.shrink_to_fit();
    face_data_.shrink_to_fit();
  }

  Triangle_soup(std::vector<Point> points, std::vector<Face> faces, std::vector<FaceData> face_data)
      : points_{std::move(points)}, faces_{std::move(faces)}, face_data_{std::move(face_data)} {}

  Vertex_handle add_vertex(const Point& p) {
    points_.push_back(p);
    return {points_.size() - 1};
  }

  Face_handle add_face(const Face& face) {
    faces_.push_back(face);
    face_data_.emplace_back();
    return {faces_.size() - 1};
  }

  void save(const std::string& filename) const {
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

  auto vertices() const { return boost::make_iterator_range(vertices_begin(), vertices_end()); }

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

  Bbox bbox() const { return CGAL::bbox_3(points_.begin(), points_.end()); }

  const AABB_tree<Leaf>& aabb_tree() const {
    std::lock_guard lock{aabb_tree_mutex_};

    if (!aabb_tree_) {
      std::vector<Leaf> leaves;
      for (auto fh : faces()) {
        leaves.emplace_back(internal::face_bbox(*this, fh), fh);
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

template <class K, class FaceData>
struct Write<Triangle_soup<K, FaceData>> {
  void operator()(std::ostream& out, const Triangle_soup<K, FaceData>& t) const {
    kigumi_write<std::int32_t>(out, t.num_vertices());
    kigumi_write<std::int32_t>(out, t.num_faces());

    for (std::size_t i = 0; i < t.num_vertices(); ++i) {
      const auto& p = t.point({i});

      if (p.approx().x().is_point() && p.approx().y().is_point() && p.approx().z().is_point()) {
        kigumi_write<bool>(out, false);
        kigumi_write<double>(out, p.approx().x().inf());
        kigumi_write<double>(out, p.approx().y().inf());
        kigumi_write<double>(out, p.approx().z().inf());
      } else {
        kigumi_write<bool>(out, true);
        kigumi_write<CGAL::Exact_rational>(out, p.exact().x());
        kigumi_write<CGAL::Exact_rational>(out, p.exact().y());
        kigumi_write<CGAL::Exact_rational>(out, p.exact().z());
      }
    }

    for (const auto& fh : t.faces()) {
      const auto& f = t.face(fh);
      const auto& f_data = t.data(fh);
      kigumi_write<Vertex_handle>(out, f[0]);
      kigumi_write<Vertex_handle>(out, f[1]);
      kigumi_write<Vertex_handle>(out, f[2]);
      kigumi_write<FaceData>(out, f_data);
    }
  }
};

template <class K, class FaceData>
struct Read<Triangle_soup<K, FaceData>> {
  void operator()(std::istream& in, Triangle_soup<K, FaceData>& t) const {
    std::size_t num_vertices{};
    std::size_t num_faces{};
    kigumi_read<std::int32_t>(in, num_vertices);
    kigumi_read<std::int32_t>(in, num_faces);

    for (std::size_t i = 0; i < num_vertices; ++i) {
      bool is_exact{};
      kigumi_read<bool>(in, is_exact);

      if (!is_exact) {
        double x{};
        double y{};
        double z{};
        kigumi_read<double>(in, x);
        kigumi_read<double>(in, y);
        kigumi_read<double>(in, z);
        t.add_vertex({x, y, z});
      } else {
        CGAL::Exact_rational x;
        CGAL::Exact_rational y;
        CGAL::Exact_rational z;
        kigumi_read<CGAL::Exact_rational>(in, x);
        kigumi_read<CGAL::Exact_rational>(in, y);
        kigumi_read<CGAL::Exact_rational>(in, z);
        t.add_vertex({CGAL::Lazy_exact_nt<CGAL::Exact_rational>{x},
                      CGAL::Lazy_exact_nt<CGAL::Exact_rational>{y},
                      CGAL::Lazy_exact_nt<CGAL::Exact_rational>{z}});
      }
    }

    for (std::size_t i = 0; i < num_faces; ++i) {
      Face face{};
      FaceData f_data{};
      kigumi_read<Vertex_handle>(in, face[0]);
      kigumi_read<Vertex_handle>(in, face[1]);
      kigumi_read<Vertex_handle>(in, face[2]);
      kigumi_read<FaceData>(in, f_data);
      auto fh = t.add_face(face);
      t.data(fh) = f_data;
    }
  }
};

}  // namespace kigumi
