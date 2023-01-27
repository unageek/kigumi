#pragma once

#include <CGAL/intersection_3.h>
#include <kigumi/Face_pair_finder.h>
#include <kigumi/Polygon_soup.h>
#include <kigumi/Triangulator.h>

#include <iterator>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Corefine {
  using Point = typename K::Point_3;
  using Segment = typename K::Segment_3;
  using Triangle = typename K::Triangle_3;
  using Intersection =
      decltype(CGAL::intersection(std::declval<Triangle>(), std::declval<Triangle>()));

 public:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  Corefine(const Polygon_soup<K, FaceData>& left, const Polygon_soup<K, FaceData>& right)
      : left_{left}, right_{right} {
    std::cout << "Finding face pairs..." << std::endl;

    Face_pair_finder finder{left_, right_};
    auto pairs = finder.find_face_pairs();

    std::cout << "Finding intersections..." << std::endl;

    std::vector<Intersection_info> infos;

#pragma omp parallel
    {
      std::vector<Intersection_info> local_infos;

#pragma omp for schedule(guided)
      for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(pairs.size()); ++i) {
        auto [left_fh, right_fh] = pairs.at(i);
        auto left_tri = left_.triangle(left_fh);
        auto right_tri = right_.triangle(right_fh);
        auto result = CGAL::intersection(left_tri, right_tri);
        if (result) {
          local_infos.emplace_back(left_fh, right_fh, std::move(result));
        }
      }

#pragma omp critical
      infos.insert(infos.end(), std::make_move_iterator(local_infos.begin()),
                   std::make_move_iterator(local_infos.end()));
    }

    std::cout << "Triangulating..." << std::endl;

    for (const auto& info : infos) {
      if (!left_triangulators_.contains(info.left_fh)) {
        auto tri = left_.triangle(info.left_fh);
        left_triangulators_.emplace(info.left_fh, Triangulator<K>(tri));
      }
      if (!right_triangulators_.contains(info.right_fh)) {
        auto tri = right_.triangle(info.right_fh);
        right_triangulators_.emplace(info.right_fh, Triangulator<K>(tri));
      }
    }

    std::sort(infos.begin(), infos.end(),
              [](const auto& a, const auto& b) { return a.left_fh < b.left_fh; });

    std::vector<std::size_t> left_fh_starts;
    for (std::size_t i = 0; i < infos.size(); ++i) {
      if (i == 0 || infos.at(i).left_fh != infos.at(i - 1).left_fh) {
        left_fh_starts.push_back(i);
      }
    }
    left_fh_starts.push_back(infos.size());

    bool caught{};
#pragma omp parallel for schedule(guided)
    for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(left_fh_starts.size() - 1); ++i) {
      if (caught) {
        continue;
      }

      auto left_fh = infos.at(left_fh_starts.at(i)).left_fh;
      auto& triangulator = left_triangulators_.at(left_fh);

      for (auto j = left_fh_starts.at(i); j < left_fh_starts.at(i + 1); ++j) {
        try {
          insert_intersection(triangulator, infos.at(j).intersection);
        } catch (const typename Triangulator<K>::Intersection_of_constraints_exception&) {
          caught = true;
          break;
        }
      }
    }

    if (caught) {
      throw std::runtime_error("the second mesh has self-intersections");
    }

    std::sort(infos.begin(), infos.end(),
              [](const auto& a, const auto& b) { return a.right_fh < b.right_fh; });

    std::vector<std::size_t> right_fh_starts;
    for (std::size_t i = 0; i < infos.size(); ++i) {
      if (i == 0 || infos.at(i).right_fh != infos.at(i - 1).right_fh) {
        right_fh_starts.push_back(i);
      }
    }
    right_fh_starts.push_back(infos.size());

#pragma omp parallel for schedule(guided)
    for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(right_fh_starts.size() - 1); ++i) {
      if (caught) {
        continue;
      }

      auto right_fh = infos.at(right_fh_starts.at(i)).right_fh;
      auto& triangulator = right_triangulators_.at(right_fh);

      for (auto j = right_fh_starts.at(i); j < right_fh_starts.at(i + 1); ++j) {
        try {
          insert_intersection(triangulator, infos.at(j).intersection);
        } catch (const typename Triangulator<K>::Intersection_of_constraints_exception&) {
          caught = true;
          break;
        }
      }
    }

    if (caught) {
      throw std::runtime_error("the first mesh has self-intersections");
    }
  }

  template <class OutputIterator>
  void get_left_triangles(Face_handle fh, OutputIterator tris) const {
    get_triangles(left_, fh, left_triangulators_, tris);
  }

  template <class OutputIterator>
  void get_right_triangles(Face_handle fh, OutputIterator tris) const {
    get_triangles(right_, fh, right_triangulators_, tris);
  }

 private:
  struct Intersection_info {
    Face_handle left_fh;
    Face_handle right_fh;
    Intersection intersection;

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    Intersection_info(Face_handle left_fh, Face_handle right_fh, Intersection&& intersection)
        : left_fh{left_fh}, right_fh{right_fh}, intersection{std::move(intersection)} {}

    Intersection_info(Intersection_info&& other) noexcept
        : left_fh{other.left_fh},
          right_fh{other.right_fh},
          intersection{std::move(other.intersection)} {}

    Intersection_info& operator=(Intersection_info&& other) noexcept {
      left_fh = other.left_fh;
      right_fh = other.right_fh;
      intersection = std::move(other.intersection);
      return *this;
    }
  };

  template <class OutputIterator>
  void get_triangles(const Polygon_soup<K, FaceData>& soup, Face_handle fh,
                     const std::unordered_map<Face_handle, Triangulator<K>>& triangulators,
                     OutputIterator tris) const {
    auto it = triangulators.find(fh);
    if (it == triangulators.end()) {
      *tris++ = soup.triangle(fh);
    } else {
      it->second.get_triangles(tris);
    }
  }

  void insert_intersection(Triangulator<K>& triangulator, const Intersection& intersection) {
    if (!intersection) {
      return;
    }

    if (const auto* point = boost::get<Point>(&*intersection)) {
      triangulator.insert(*point);
    } else if (const auto* s = boost::get<Segment>(&*intersection)) {
      auto p = s->source();
      auto q = s->target();
      auto vh0 = triangulator.insert(p);
      auto vh1 = triangulator.insert(q);
      triangulator.insert_constraint(vh0, vh1);
    } else if (const auto* t = boost::get<Triangle>(&*intersection)) {
      auto p = t->vertex(0);
      auto q = t->vertex(1);
      auto r = t->vertex(2);
      auto vh0 = triangulator.insert(p);
      auto vh1 = triangulator.insert(q);
      auto vh2 = triangulator.insert(r);
      triangulator.insert_constraint(vh0, vh1);
      triangulator.insert_constraint(vh1, vh2);
      triangulator.insert_constraint(vh2, vh0);
    } else if (const auto* points = boost::get<std::vector<Point>>(&*intersection)) {
      std::vector<typename Triangulator<K>::Vertex_handle> vhs;
      // Four to six points.
      for (const auto& p : *points) {
        vhs.push_back(triangulator.insert(p));
      }
      for (std::size_t i = 0; i < vhs.size(); ++i) {
        triangulator.insert_constraint(vhs.at(i), vhs.at((i + 1) % vhs.size()));
      }
    }
  }

  const Polygon_soup<K, FaceData>& left_;
  std::unordered_map<Face_handle, Triangulator<K>> left_triangulators_;
  const Polygon_soup<K, FaceData>& right_;
  std::unordered_map<Face_handle, Triangulator<K>> right_triangulators_;
};

}  // namespace kigumi
