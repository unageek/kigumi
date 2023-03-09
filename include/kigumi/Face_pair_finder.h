#pragma once

#include <kigumi/AABB_tree/Coincide.h>
#include <kigumi/AABB_tree/Overlap.h>
#include <kigumi/Triangle_soup.h>

#include <iterator>
#include <unordered_set>
#include <utility>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Face_pair_finder {
  using Face_pair = std::pair<Face_handle, Face_handle>;
  using Leaf = typename Triangle_soup<K, FaceData>::Leaf;

 public:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  Face_pair_finder(const Triangle_soup<K, FaceData>& left, const Triangle_soup<K, FaceData>& right)
      : left_{left}, right_{right} {}

  std::vector<Face_pair> find_face_pairs() const {
    std::unordered_set<Face_handle> left_fhs_to_skip;
    std::unordered_set<Face_handle> right_fhs_to_skip;
    {
      std::vector<Face_pair> pairs;

#pragma omp parallel
      {
        std::vector<Face_pair> local_pairs;
        std::vector<const Leaf*> leaves;

        if (left_.num_faces() < right_.num_faces()) {
          const auto& left_tree = left_.aabb_tree();

#pragma omp for schedule(guided)
          for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(right_.num_faces()); ++i) {
            Face_handle fh{static_cast<std::size_t>(i)};
            auto right_tri = right_.triangle(fh);
            leaves.clear();
            left_tree.template get_intersecting_leaves<Coincide<K>>(std::back_inserter(leaves),
                                                                    right_.triangle(fh));
            for (const auto* leaf : leaves) {
              auto left_tri = left_.triangle(leaf->face_handle());
              if (left_tri[0] == right_tri[0] &&
                  ((left_tri[1] == right_tri[1] && left_tri[2] == right_tri[2]) ||
                   (left_tri[1] == right_tri[2] && left_tri[2] == right_tri[1]))) {
                local_pairs.emplace_back(leaf->face_handle(), fh);
              }
            }
          }
        } else {
          const auto& right_tree = right_.aabb_tree();

#pragma omp for schedule(guided)
          for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(left_.num_faces()); ++i) {
            Face_handle fh{static_cast<std::size_t>(i)};
            auto left_tri = left_.triangle(fh);
            leaves.clear();
            right_tree.template get_intersecting_leaves<Coincide<K>>(std::back_inserter(leaves),
                                                                     left_.triangle(fh));
            for (const auto* leaf : leaves) {
              auto right_tri = right_.triangle(leaf->face_handle());
              if (left_tri[0] == right_tri[0] &&
                  ((left_tri[1] == right_tri[1] && left_tri[2] == right_tri[2]) ||
                   (left_tri[1] == right_tri[2] && left_tri[2] == right_tri[1]))) {
                local_pairs.emplace_back(fh, leaf->face_handle());
              }
            }
          }
        }

#pragma omp critical
        pairs.insert(pairs.end(), local_pairs.begin(), local_pairs.end());
      }

      for (const auto& [left_fh, right_fh] : pairs) {
        left_fhs_to_skip.insert(left_fh);
        right_fhs_to_skip.insert(right_fh);
      }
    }

    {
      std::vector<Face_pair> pairs;

#pragma omp parallel
      {
        std::vector<Face_pair> local_pairs;
        std::vector<const Leaf*> leaves;

        if (left_.num_faces() < right_.num_faces()) {
          const auto& left_tree = left_.aabb_tree();

#pragma omp for schedule(guided)
          for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(right_.num_faces()); ++i) {
            Face_handle fh{static_cast<std::size_t>(i)};
            if (right_fhs_to_skip.contains(fh)) {
              continue;
            }

            leaves.clear();
            left_tree.template get_intersecting_leaves<Overlap<K>>(std::back_inserter(leaves),
                                                                   right_.triangle(fh));
            for (const auto* leaf : leaves) {
              if (left_fhs_to_skip.contains(leaf->face_handle())) {
                continue;
              }

              local_pairs.emplace_back(leaf->face_handle(), fh);
            }
          }
        } else {
          const auto& right_tree = right_.aabb_tree();

#pragma omp for schedule(guided)
          for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(left_.num_faces()); ++i) {
            Face_handle fh{static_cast<std::size_t>(i)};
            if (left_fhs_to_skip.contains(fh)) {
              continue;
            }

            leaves.clear();
            right_tree.template get_intersecting_leaves<Overlap<K>>(std::back_inserter(leaves),
                                                                    left_.triangle(fh));
            for (const auto* leaf : leaves) {
              if (right_fhs_to_skip.contains(leaf->face_handle())) {
                continue;
              }

              local_pairs.emplace_back(fh, leaf->face_handle());
            }
          }
        }

#pragma omp critical
        pairs.insert(pairs.end(), local_pairs.begin(), local_pairs.end());
      }

      return pairs;
    }
  }

 private:
  const Triangle_soup<K, FaceData>& left_;
  const Triangle_soup<K, FaceData>& right_;
};

}  // namespace kigumi
