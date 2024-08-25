#pragma once

#include <kigumi/Face_face_intersection.h>
#include <kigumi/Find_coplanar_faces.h>
#include <kigumi/Find_possibly_intersecting_faces.h>
#include <kigumi/Intersection_point_inserter.h>
#include <kigumi/Mesh_handles.h>
#include <kigumi/Point_list.h>
#include <kigumi/Triangle_region.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Triangulator.h>
#include <kigumi/parallel_do.h>

#include <algorithm>
#include <boost/container/static_vector.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Corefine {
  using Face_face_intersection = Face_face_intersection<K>;
  using Find_coplanar_faces = Find_coplanar_faces<K, FaceData>;
  using Find_possibly_intersecting_faces = Find_possibly_intersecting_faces<K, FaceData>;
  using Intersection_point_inserter = Intersection_point_inserter<K>;
  using Point = typename K::Point_3;
  using Point_list = Point_list<K>;
  using Triangle_soup = Triangle_soup<K, FaceData>;
  using Triangulator = Triangulator<K>;

 public:
  Corefine(const Triangle_soup& left, const Triangle_soup& right) : left_{left}, right_{right} {
    std::cout << "Finding face pairs..." << std::endl;

    points_.reserve(left_.num_vertices() + right_.num_vertices());
    for (auto vh : left_.vertices()) {
      left_point_ids_.push_back(points_.insert(left_.point(vh)));
    }
    for (auto vh : right_.vertices()) {
      right_point_ids_.push_back(points_.insert(right_.point(vh)));
    }

    auto cop = Find_coplanar_faces{}(left_, right_, left_point_ids_, right_point_ids_);
    std::unordered_set<Face_handle> left_faces_to_ignore;
    left_faces_to_ignore.insert(cop.left_coplanar_faces.begin(), cop.left_coplanar_faces.end());
    left_faces_to_ignore.insert(cop.left_opposite_faces.begin(), cop.left_opposite_faces.end());
    std::unordered_set<Face_handle> right_faces_to_ignore;
    right_faces_to_ignore.insert(cop.right_coplanar_faces.begin(), cop.right_coplanar_faces.end());
    right_faces_to_ignore.insert(cop.right_opposite_faces.begin(), cop.right_opposite_faces.end());

    auto pairs = Find_possibly_intersecting_faces{}(left_, right_, left_faces_to_ignore,
                                                    right_faces_to_ignore);

    std::cout << "Finding symbolic intersections..." << std::endl;

    std::vector<Intersection_info> infos;

    std::size_t num_intersections{};
    parallel_do(
        pairs.begin(), pairs.end(), std::pair<std::vector<Intersection_info>, std::size_t>{},
        [&](const auto& pair, auto& local_state) {
          thread_local Face_face_intersection face_face_intersection{points_};

          auto [left_fh, right_fh] = pair;
          const auto& left_face = left_.face(left_fh);
          const auto& right_face = right_.face(right_fh);
          auto a = left_point_ids_.at(left_face[0].i);
          auto b = left_point_ids_.at(left_face[1].i);
          auto c = left_point_ids_.at(left_face[2].i);
          auto p = right_point_ids_.at(right_face[0].i);
          auto q = right_point_ids_.at(right_face[1].i);
          auto r = right_point_ids_.at(right_face[2].i);
          auto sym_inters = face_face_intersection(a, b, c, p, q, r);
          if (sym_inters.empty()) {
            return;
          }
          auto& [local_infos, local_num_intersections] = local_state;
          local_infos.emplace_back(left_fh, right_fh, sym_inters);
          local_num_intersections += sym_inters.size();
        },
        [&](auto& local_state) {
          auto& [local_infos, local_num_intersections] = local_state;
          if (infos.empty()) {
            infos = std::move(local_infos);
          } else {
            infos.insert(infos.end(), local_infos.begin(), local_infos.end());
          }
          num_intersections += local_num_intersections;
        });

    std::cout << "Constructing intersection points..." << std::endl;

    points_.reserve(points_.size() + num_intersections / 2);
    Intersection_point_inserter inserter(points_);
    for (auto& info : infos) {
      const auto& left_face = left_.face(info.left_fh);
      const auto& right_face = right_.face(info.right_fh);
      auto a = left_point_ids_.at(left_face[0].i);
      auto b = left_point_ids_.at(left_face[1].i);
      auto c = left_point_ids_.at(left_face[2].i);
      auto p = right_point_ids_.at(right_face[0].i);
      auto q = right_point_ids_.at(right_face[1].i);
      auto r = right_point_ids_.at(right_face[2].i);
      for (auto sym_inter : info.symbolic_intersections) {
        auto id = inserter.insert(sym_inter.first, a, b, c, sym_inter.second, p, q, r);
        info.intersections.push_back(id);
      }
    }

    std::cout << "Triangulating..." << std::endl;

    for (const auto& info : infos) {
      if (!left_triangulators_.contains(info.left_fh)) {
        auto fh = info.left_fh;
        const auto& f = left_.face(fh);
        auto tri = left_.triangle(fh);
        auto a = left_point_ids_.at(f[0].i);
        auto b = left_point_ids_.at(f[1].i);
        auto c = left_point_ids_.at(f[2].i);
        left_triangulators_.emplace(fh, Triangulator{tri, {a, b, c}});
      }
      if (!right_triangulators_.contains(info.right_fh)) {
        auto fh = info.right_fh;
        const auto& f = right_.face(fh);
        auto tri = right_.triangle(fh);
        auto a = right_point_ids_.at(f[0].i);
        auto b = right_point_ids_.at(f[1].i);
        auto c = right_point_ids_.at(f[2].i);
        right_triangulators_.emplace(fh, Triangulator{tri, {a, b, c}});
      }
    }

    std::sort(infos.begin(), infos.end(),
              [](const auto& a, const auto& b) { return a.left_fh < b.left_fh; });

    std::vector<typename decltype(infos)::const_iterator> partitions;
    for (auto it = infos.begin(); it != infos.end(); ++it) {
      if (it == infos.begin() || it->left_fh != std::prev(it)->left_fh) {
        partitions.push_back(it);
      }
    }
    partitions.push_back(infos.end());

    try {
      parallel_do(boost::make_zip_iterator(
                      boost::make_tuple(partitions.begin(), std::next(partitions.begin()))),
                  boost::make_zip_iterator(
                      boost::make_tuple(std::prev(partitions.end()), partitions.end())),
                  [&](const auto& partition_range) {
                    auto begin = partition_range.template get<0>();
                    auto end = partition_range.template get<1>();
                    auto& triangulator = left_triangulators_.at(begin->left_fh);
                    for (auto it = begin; it != end; ++it) {
                      insert_intersection(triangulator, *it);
                    }
                  });
    } catch (const typename Triangulator::Intersection_of_constraints_exception&) {
      throw std::runtime_error("the second mesh has self-intersections");
    }

    std::sort(infos.begin(), infos.end(),
              [](const auto& a, const auto& b) { return a.right_fh < b.right_fh; });

    partitions.clear();
    for (auto it = infos.begin(); it != infos.end(); ++it) {
      if (it == infos.begin() || it->right_fh != std::prev(it)->right_fh) {
        partitions.push_back(it);
      }
    }
    partitions.push_back(infos.end());

    try {
      parallel_do(boost::make_zip_iterator(
                      boost::make_tuple(partitions.begin(), std::next(partitions.begin()))),
                  boost::make_zip_iterator(
                      boost::make_tuple(std::prev(partitions.end()), partitions.end())),
                  [&](const auto& partition_range) {
                    auto begin = partition_range.template get<0>();
                    auto end = partition_range.template get<1>();
                    auto& triangulator = right_triangulators_.at(begin->right_fh);
                    for (auto it = begin; it != end; ++it) {
                      insert_intersection(triangulator, *it);
                    }
                  });
    } catch (const typename Triangulator::Intersection_of_constraints_exception&) {
      throw std::runtime_error("the first mesh has self-intersections");
    }
  }

  template <class OutputIterator>
  void get_left_triangles(Face_handle fh, OutputIterator tris) const {
    get_triangles(left_, fh, left_triangulators_, left_point_ids_, tris);
  }

  template <class OutputIterator>
  void get_right_triangles(Face_handle fh, OutputIterator tris) const {
    get_triangles(right_, fh, right_triangulators_, right_point_ids_, tris);
  }

  std::vector<Point> take_points() { return points_.take_points(); }

 private:
  struct Intersection_info {
    Face_handle left_fh;
    Face_handle right_fh;
    boost::container::static_vector<std::pair<TriangleRegion, TriangleRegion>, 6>
        symbolic_intersections;
    boost::container::static_vector<std::size_t, 6> intersections;
  };

  template <class OutputIterator>
  void get_triangles(const Triangle_soup& soup, Face_handle fh,
                     const std::unordered_map<Face_handle, Triangulator>& triangulators,
                     const std::vector<std::size_t>& point_ids, OutputIterator tris) const {
    auto it = triangulators.find(fh);
    if (it == triangulators.end()) {
      const auto& f = soup.face(fh);
      auto a = point_ids.at(f[0].i);
      auto b = point_ids.at(f[1].i);
      auto c = point_ids.at(f[2].i);
      *tris++ = {a, b, c};
    } else {
      it->second.get_triangles(tris);
    }
  }

  void insert_intersection(Triangulator& triangulator, const Intersection_info& info) {
    typename Triangulator::Vertex_handle null_vh;
    auto prev = null_vh;
    for (auto id : info.intersections) {
      auto cur = triangulator.insert(points_.at(id), id);
      if (prev != null_vh) {
        triangulator.insert_constraint(prev, cur);
      }
      prev = cur;
    }
  }

  const Triangle_soup& left_;
  std::unordered_map<Face_handle, Triangulator> left_triangulators_;
  const Triangle_soup& right_;
  std::unordered_map<Face_handle, Triangulator> right_triangulators_;
  Point_list points_;
  std::vector<std::size_t> left_point_ids_;
  std::vector<std::size_t> right_point_ids_;
};

}  // namespace kigumi
