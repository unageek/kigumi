#pragma once

#include <kigumi/Face_face_intersection.h>
#include <kigumi/Face_tag.h>
#include <kigumi/Find_coplanar_faces.h>
#include <kigumi/Find_possibly_intersecting_faces.h>
#include <kigumi/Intersection_point_inserter.h>
#include <kigumi/Mesh_entities.h>
#include <kigumi/Mesh_indices.h>
#include <kigumi/Point_list.h>
#include <kigumi/Triangle_region.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Triangulation.h>
#include <kigumi/parallel_do.h>

#include <algorithm>
#include <boost/container/static_vector.hpp>
#include <boost/range/iterator_range.hpp>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
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
  using Triangulation = Triangulation<K>;

 public:
  Corefine(const Triangle_soup& left, const Triangle_soup& right) : left_{left}, right_{right} {
    std::cout << "Finding face pairs..." << std::endl;

    points_.start_uniqueness_check();
    points_.reserve(left_.num_vertices() + right_.num_vertices());
    for (auto vi : left_.vertices()) {
      left_point_ids_.push_back(points_.insert(left_.point(vi)));
    }
    for (auto vi : right_.vertices()) {
      right_point_ids_.push_back(points_.insert(right_.point(vi)));
    }
    points_.stop_uniqueness_check();

    std::tie(left_face_tags_, right_face_tags_) =
        Find_coplanar_faces{}(left_, right_, left_point_ids_, right_point_ids_);

    auto pairs =
        Find_possibly_intersecting_faces{}(left_, right_, left_face_tags_, right_face_tags_);

    std::cout << "Finding symbolic intersections..." << std::endl;

    std::size_t num_intersections{};
    parallel_do(
        pairs.begin(), pairs.end(), std::pair<std::vector<Intersection_info>, std::size_t>{},
        [&](const auto& pair, auto& local_state) {
          thread_local Face_face_intersection face_face_intersection{points_};

          auto [left_fi, right_fi] = pair;
          const auto& left_face = left_.face(left_fi);
          const auto& right_face = right_.face(right_fi);
          auto a = left_point_ids_.at(left_face[0].idx());
          auto b = left_point_ids_.at(left_face[1].idx());
          auto c = left_point_ids_.at(left_face[2].idx());
          auto p = right_point_ids_.at(right_face[0].idx());
          auto q = right_point_ids_.at(right_face[1].idx());
          auto r = right_point_ids_.at(right_face[2].idx());
          auto sym_inters = face_face_intersection(a, b, c, p, q, r);
          if (sym_inters.empty()) {
            return;
          }
          auto& [local_infos, local_num_intersections] = local_state;
          local_infos.emplace_back(left_fi, right_fi, sym_inters);
          local_num_intersections += sym_inters.size();
        },
        [&](auto& local_state) {
          auto& [local_infos, local_num_intersections] = local_state;
          if (infos_.empty()) {
            infos_ = std::move(local_infos);
          } else {
            infos_.insert(infos_.end(), local_infos.begin(), local_infos.end());
          }
          num_intersections += local_num_intersections;
        });

    std::cout << "Constructing intersection points..." << std::endl;

    auto num_points_before_insertion = points_.size();

    points_.reserve(num_points_before_insertion + num_intersections / 2);
    Intersection_point_inserter inserter(points_);
    for (auto& info : infos_) {
      const auto& left_face = left_.face(info.left_fi);
      const auto& right_face = right_.face(info.right_fi);
      auto a = left_point_ids_.at(left_face[0].idx());
      auto b = left_point_ids_.at(left_face[1].idx());
      auto c = left_point_ids_.at(left_face[2].idx());
      auto p = right_point_ids_.at(right_face[0].idx());
      auto q = right_point_ids_.at(right_face[1].idx());
      auto r = right_point_ids_.at(right_face[2].idx());
      for (auto sym_inter : info.symbolic_intersections) {
        auto left_region = intersection(sym_inter, Triangle_region::LEFT_FACE);
        auto right_region = intersection(sym_inter, Triangle_region::RIGHT_FACE);
        auto id = inserter.insert(left_region, a, b, c, right_region, p, q, r);
        info.intersections.push_back(id);
      }
    }

    parallel_do(points_.begin() + num_points_before_insertion, points_.end(),
                [](const auto& p) { p.exact(); });

    std::cout << "Triangulating..." << std::endl;

    auto left_fi_less = [](const Intersection_info& a, const Intersection_info& b) -> bool {
      return a.left_fi < b.left_fi;
    };
    std::sort(infos_.begin(), infos_.end(), left_fi_less);

    std::vector<boost::iterator_range<typename decltype(infos_)::const_iterator>> ranges;
    {
      auto first = infos_.begin();
      while (first != infos_.end()) {
        auto fi = first->left_fi;
        const auto& f = left_.face(fi);
        auto a = left_point_ids_.at(f[0].idx());
        auto b = left_point_ids_.at(f[1].idx());
        auto c = left_point_ids_.at(f[2].idx());
        const auto& pa = points_.at(a);
        const auto& pb = points_.at(b);
        const auto& pc = points_.at(c);
        left_triangulations_.emplace(
            fi, Triangulation{Triangle_region::LEFT_FACE, pa, pb, pc, a, b, c});

        auto last = std::upper_bound(first, infos_.end(), *first, left_fi_less);
        ranges.emplace_back(first, last);
        first = last;
      }
    }

    try {
      parallel_do(ranges.begin(), ranges.end(), [&](const auto& range) {
        const auto& any_info = range.front();
        auto& triangulation = left_triangulations_.at(any_info.left_fi);
        for (const auto& info : range) {
          insert_intersection(triangulation, info);
        }
      });
    } catch (const typename Triangulation::Intersection_of_constraints_exception&) {
      throw std::runtime_error("the second mesh has self-intersections");
    }

    auto right_fi_less = [](const Intersection_info& a, const Intersection_info& b) -> bool {
      return a.right_fi < b.right_fi;
    };
    std::sort(infos_.begin(), infos_.end(), right_fi_less);

    ranges.clear();
    {
      auto first = infos_.begin();
      while (first != infos_.end()) {
        auto fi = first->right_fi;
        const auto& f = right_.face(fi);
        auto a = right_point_ids_.at(f[0].idx());
        auto b = right_point_ids_.at(f[1].idx());
        auto c = right_point_ids_.at(f[2].idx());
        const auto& pa = points_.at(a);
        const auto& pb = points_.at(b);
        const auto& pc = points_.at(c);
        right_triangulations_.emplace(
            fi, Triangulation{Triangle_region::RIGHT_FACE, pa, pb, pc, a, b, c});

        auto last = std::upper_bound(first, infos_.end(), *first, right_fi_less);
        ranges.emplace_back(first, last);
        first = last;
      }
    }

    try {
      parallel_do(ranges.begin(), ranges.end(), [&](const auto& range) {
        const auto& any_info = range.front();
        auto& triangulation = right_triangulations_.at(any_info.right_fi);
        for (const auto& info : range) {
          insert_intersection(triangulation, info);
        }
      });
    } catch (const typename Triangulation::Intersection_of_constraints_exception&) {
      throw std::runtime_error("the first mesh has self-intersections");
    }
  }

  std::vector<Edge> get_intersecting_edges() const {
    std::vector<Edge> edges;

    for (const auto& info : infos_) {
      auto n = info.intersections.size();
      if (n < 2) {
        continue;
      }

      for (std::size_t i = 0; i < n; ++i) {
        auto j = i < n - 1 ? i + 1 : 0;
        auto a = info.intersections.at(i);
        auto b = info.intersections.at(j);
        edges.push_back(make_edge(Vertex_index{a}, Vertex_index{b}));
      }
    }

    return edges;
  }

  template <class OutputIterator>
  Face_tag get_left_triangles(Face_index fi, OutputIterator tris) const {
    get_triangles(left_, fi, left_triangulations_, left_point_ids_, tris);
    return left_face_tags_.at(fi.idx());
  }

  template <class OutputIterator>
  Face_tag get_right_triangles(Face_index fi, OutputIterator tris) const {
    get_triangles(right_, fi, right_triangulations_, right_point_ids_, tris);
    return right_face_tags_.at(fi.idx());
  }

  std::vector<Point> take_points() { return points_.take_points(); }

 private:
  struct Intersection_info {
    Face_index left_fi;
    Face_index right_fi;
    boost::container::static_vector<Triangle_region, 6> symbolic_intersections;
    boost::container::static_vector<std::size_t, 6> intersections;
  };

  template <class OutputIterator>
  void get_triangles(const Triangle_soup& soup, Face_index fi,
                     const std::unordered_map<Face_index, Triangulation>& triangulations,
                     const std::vector<std::size_t>& point_ids, OutputIterator tris) const {
    auto it = triangulations.find(fi);
    if (it == triangulations.end()) {
      const auto& f = soup.face(fi);
      auto a = point_ids.at(f[0].idx());
      auto b = point_ids.at(f[1].idx());
      auto c = point_ids.at(f[2].idx());
      *tris++ = {a, b, c};
    } else {
      it->second.get_triangles(tris);
    }
  }

  void insert_intersection(Triangulation& triangulation, const Intersection_info& info) {
    typename Triangulation::Vertex_handle null_vh;
    auto first = null_vh;
    auto prev = null_vh;
    for (std::size_t i = 0; i < info.intersections.size(); ++i) {
      auto id = info.intersections.at(i);
      auto sym = info.symbolic_intersections.at(i);
      auto cur = triangulation.insert(points_.at(id), id, sym);
      if (prev != null_vh) {
        triangulation.insert_constraint(prev, cur);
      }
      if (first == null_vh) {
        first = cur;
      }
      prev = cur;
    }
    if (info.intersections.size() > 2) {
      triangulation.insert_constraint(prev, first);
    }
  }

  const Triangle_soup& left_;
  std::unordered_map<Face_index, Triangulation> left_triangulations_;
  const Triangle_soup& right_;
  std::unordered_map<Face_index, Triangulation> right_triangulations_;
  Point_list points_;
  std::vector<std::size_t> left_point_ids_;
  std::vector<std::size_t> right_point_ids_;
  std::vector<Face_tag> left_face_tags_;
  std::vector<Face_tag> right_face_tags_;
  std::vector<Intersection_info> infos_;
};

}  // namespace kigumi
