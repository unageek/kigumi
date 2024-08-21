#pragma once

#include <kigumi/AABB_tree/Overlap.h>
#include <kigumi/Mesh_handles.h>
#include <kigumi/Triangle_soup.h>

#include <iterator>
#include <unordered_set>
#include <utility>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Find_possibly_intersecting_faces {
  using Face_handle_pair = std::pair<Face_handle, Face_handle>;
  using Overlap = Overlap<K>;
  using Triangle_soup = Triangle_soup<K, FaceData>;
  using Leaf = typename Triangle_soup::Leaf;

 public:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  std::vector<Face_handle_pair> operator()(
      const Triangle_soup& left, const Triangle_soup& right,
      const std::unordered_set<Face_handle>& left_faces_to_ignore,
      const std::unordered_set<Face_handle>& right_faces_to_ignore) const {
    std::vector<Face_handle_pair> pairs;

#pragma omp parallel
    {
      std::vector<Face_handle_pair> local_pairs;
      std::vector<const Leaf*> leaves;

      if (left.num_faces() < right.num_faces()) {
        const auto& left_tree = left.aabb_tree();

#pragma omp for schedule(guided)
        for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(right.num_faces()); ++i) {
          Face_handle right_fh{static_cast<std::size_t>(i)};
          if (right_faces_to_ignore.contains(right_fh)) {
            continue;
          }

          leaves.clear();
          left_tree.template get_intersecting_leaves<Overlap>(std::back_inserter(leaves),
                                                              right.triangle(right_fh).bbox());

          for (const auto* leaf : leaves) {
            auto left_fh = leaf->face_handle();
            if (!left_faces_to_ignore.contains(left_fh)) {
              local_pairs.emplace_back(left_fh, right_fh);
            }
          }
        }
      } else {
        const auto& right_tree = right.aabb_tree();

#pragma omp for schedule(guided)
        for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(left.num_faces()); ++i) {
          Face_handle left_fh{static_cast<std::size_t>(i)};
          if (left_faces_to_ignore.contains(left_fh)) {
            continue;
          }

          leaves.clear();
          right_tree.template get_intersecting_leaves<Overlap>(std::back_inserter(leaves),
                                                               left.triangle(left_fh).bbox());

          for (const auto* leaf : leaves) {
            auto right_fh = leaf->face_handle();
            if (!right_faces_to_ignore.contains(right_fh)) {
              local_pairs.emplace_back(left_fh, right_fh);
            }
          }
        }
      }

#pragma omp critical
      pairs.insert(pairs.end(), local_pairs.begin(), local_pairs.end());
    }

    return pairs;
  }
};

}  // namespace kigumi
