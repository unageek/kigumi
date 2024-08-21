#pragma once

#include <kigumi/Mesh_handles.h>
#include <kigumi/Triangle_soup.h>

#include <algorithm>
#include <array>
#include <boost/container_hash/hash.hpp>
#include <unordered_map>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Find_coplanar_faces {
  using Triangle = std::array<std::size_t, 3>;
  using Triangle_hash = boost::hash<Triangle>;
  using Triangle_soup = Triangle_soup<K, FaceData>;

  struct Result {
    std::vector<Face_handle> left_coplanar_faces;
    std::vector<Face_handle> left_opposite_faces;
    std::vector<Face_handle> right_coplanar_faces;
    std::vector<Face_handle> right_opposite_faces;
  };

 public:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  Result operator()(const Triangle_soup& left, const Triangle_soup& right,
                    const std::vector<std::size_t>& left_points,
                    const std::vector<std::size_t>& right_points) const {
    Result result;

    bool left_is_a = left.num_faces() < right.num_faces();
    const auto& a = left_is_a ? left : right;
    const auto& b = left_is_a ? right : left;
    const auto& a_points = left_is_a ? left_points : right_points;
    const auto& b_points = left_is_a ? right_points : left_points;
    auto& a_coplanar_faces = left_is_a ? result.left_coplanar_faces : result.right_coplanar_faces;
    auto& b_coplanar_faces = left_is_a ? result.right_coplanar_faces : result.left_coplanar_faces;
    auto& a_opposite_faces = left_is_a ? result.left_opposite_faces : result.right_opposite_faces;
    auto& b_opposite_faces = left_is_a ? result.right_opposite_faces : result.left_opposite_faces;

    std::unordered_map<Triangle, Face_handle, Triangle_hash> triangle_to_fh;

    for (auto fh : a.faces()) {
      auto tri = triangle(a, fh, a_points);
      triangle_to_fh.emplace(tri, fh);
    }

    for (auto fh : b.faces()) {
      auto tri = triangle(b, fh, b_points);

      auto it = triangle_to_fh.find(tri);
      if (it != triangle_to_fh.end()) {
        a_coplanar_faces.push_back(it->second);
        b_coplanar_faces.push_back(fh);
        continue;
      }

      it = triangle_to_fh.find(opposite(tri));
      if (it != triangle_to_fh.end()) {
        a_opposite_faces.push_back(it->second);
        b_opposite_faces.push_back(fh);
      }
    }

    return result;
  }

 private:
  static Triangle triangle(const Triangle_soup& m, Face_handle fh,
                           const std::vector<std::size_t>& points) {
    auto face = m.face(fh);
    Triangle triangle{
        points.at(face[0].i),
        points.at(face[1].i),
        points.at(face[2].i),
    };
    std::rotate(triangle.begin(), std::min_element(triangle.begin(), triangle.end()),
                triangle.end());
    return triangle;
  }

  static Triangle opposite(const Triangle& triangle) {
    return {triangle[0], triangle[2], triangle[1]};
  }
};

}  // namespace kigumi
