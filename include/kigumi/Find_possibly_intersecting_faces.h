#pragma once

#include <kigumi/Mesh_handles.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/mesh_utility.h>
#include <kigumi/parallel_do.h>

#include <iterator>
#include <unordered_set>
#include <utility>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Find_possibly_intersecting_faces {
  using Face_handle_pair = std::pair<Face_handle, Face_handle>;
  using Triangle_soup = Triangle_soup<K, FaceData>;
  using Leaf = typename Triangle_soup::Leaf;

 public:
  std::vector<Face_handle_pair> operator()(
      const Triangle_soup& left, const Triangle_soup& right,
      const std::unordered_set<Face_handle>& left_faces_to_ignore,
      const std::unordered_set<Face_handle>& right_faces_to_ignore) const {
    std::vector<Face_handle_pair> pairs;

    auto left_is_a = left.num_faces() < right.num_faces();
    const auto& a = left_is_a ? left : right;
    const auto& b = left_is_a ? right : left;
    const auto& a_faces_to_ignore = left_is_a ? left_faces_to_ignore : right_faces_to_ignore;
    const auto& b_faces_to_ignore = left_is_a ? right_faces_to_ignore : left_faces_to_ignore;
    const auto& a_tree = a.aabb_tree();

    parallel_do(
        b.faces_begin(), b.faces_end(),
        std::pair<std::vector<Face_handle_pair>, std::vector<const Leaf*>>{},
        [&](auto b_fh, auto& local_state) {
          auto& [local_pairs, leaves] = local_state;

          if (b_faces_to_ignore.contains(b_fh)) {
            return;
          }

          leaves.clear();
          a_tree.get_intersecting_leaves(std::back_inserter(leaves), internal::face_bbox(b, b_fh));

          for (const auto* leaf : leaves) {
            auto a_fh = leaf->face_handle();
            if (a_faces_to_ignore.contains(a_fh)) {
              continue;
            }

            if (left_is_a) {
              local_pairs.emplace_back(a_fh, b_fh);
            } else {
              local_pairs.emplace_back(b_fh, a_fh);
            }
          }
        },
        [&](auto& local_state) {
          auto& [local_pairs, leaves] = local_state;

          if (pairs.empty()) {
            pairs = std::move(local_pairs);
          } else {
            pairs.insert(pairs.end(), local_pairs.begin(), local_pairs.end());
          }
        });

    return pairs;
  }
};

}  // namespace kigumi
