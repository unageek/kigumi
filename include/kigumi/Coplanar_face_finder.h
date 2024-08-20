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
class Coplanar_face_finder {
  using Triangle_soup = Triangle_soup<K, FaceData>;
  using Triple = std::array<std::size_t, 3>;
  using Triple_hash = boost::hash<Triple>;

 public:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  Coplanar_face_finder(const Triangle_soup& left, const Triangle_soup& right,
                       const std::vector<std::size_t>& left_point_ids,
                       const std::vector<std::size_t>& right_point_ids)
      : left_(left),
        right_(right),
        left_point_ids_(left_point_ids),
        right_point_ids_(right_point_ids) {
    std::unordered_map<Triple, Face_handle, Triple_hash> triple_to_fh;

    if (left_.num_faces() < right_.num_faces()) {
      for (auto fh : left_.faces()) {
        auto triple = left_triple(fh);
        triple_to_fh.emplace(triple, fh);
      }
      for (auto fh : right_.faces()) {
        auto triple = right_triple(fh);
        auto it = triple_to_fh.find(triple);
        if (it != triple_to_fh.end()) {
          left_coplanar_faces_.push_back(it->second);
          right_coplanar_faces_.push_back(fh);
          continue;
        }
        it = triple_to_fh.find(opposite(triple));
        if (it != triple_to_fh.end()) {
          left_opposite_faces_.push_back(it->second);
          right_opposite_faces_.push_back(fh);
        }
      }
    } else {
      for (auto fh : right_.faces()) {
        auto triple = right_triple(fh);
        triple_to_fh.emplace(triple, fh);
      }
      for (auto fh : left_.faces()) {
        auto triple = left_triple(fh);
        auto it = triple_to_fh.find(triple);
        if (it != triple_to_fh.end()) {
          left_coplanar_faces_.push_back(fh);
          right_coplanar_faces_.push_back(it->second);
          continue;
        }
        it = triple_to_fh.find(opposite(triple));
        if (it != triple_to_fh.end()) {
          left_opposite_faces_.push_back(fh);
          right_opposite_faces_.push_back(it->second);
        }
      }
    }
  }

  std::vector<Face_handle> take_left_coplanar_faces() const {
    return std::move(left_coplanar_faces_);
  }

  std::vector<Face_handle> take_left_opposite_faces() const {
    return std::move(left_opposite_faces_);
  }

  std::vector<Face_handle> take_right_coplanar_faces() const {
    return std::move(right_coplanar_faces_);
  }

  std::vector<Face_handle> take_right_opposite_faces() const {
    return std::move(right_opposite_faces_);
  }

 private:
  Triple left_triple(Face_handle fh) {
    auto face = left_.face(fh);
    Triple triple{
        left_point_ids_.at(face[0].i),
        left_point_ids_.at(face[1].i),
        left_point_ids_.at(face[2].i),
    };
    std::rotate(triple.begin(), std::min_element(triple.begin(), triple.end()), triple.end());
    return triple;
  }

  Triple right_triple(Face_handle fh) {
    auto face = right_.face(fh);
    Triple triple{
        right_point_ids_.at(face[0].i),
        right_point_ids_.at(face[1].i),
        right_point_ids_.at(face[2].i),
    };
    std::rotate(triple.begin(), std::min_element(triple.begin(), triple.end()), triple.end());
    return triple;
  }

  static Triple opposite(const Triple& triple) { return {triple[0], triple[2], triple[1]}; }

  const Triangle_soup& left_;
  const Triangle_soup& right_;
  const std::vector<std::size_t>& left_point_ids_;
  const std::vector<std::size_t>& right_point_ids_;
  std::vector<Face_handle> left_coplanar_faces_;
  std::vector<Face_handle> left_opposite_faces_;
  std::vector<Face_handle> right_coplanar_faces_;
  std::vector<Face_handle> right_opposite_faces_;
};

}  // namespace kigumi
