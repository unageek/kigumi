#pragma once

#include <kigumi/Mesh_handles.h>
#include <kigumi/Triangle_soup.h>

#include <atomic>
#include <iterator>
#include <mutex>
#include <thread>
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
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
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

    std::vector<std::thread> threads;
    std::atomic<std::size_t> next_index{};
    std::mutex mutex;
    auto num_threads = std::thread::hardware_concurrency();
    for (unsigned tid = 0; tid < num_threads; ++tid) {
      threads.emplace_back([&] {
        std::vector<Face_handle_pair> local_pairs;
        std::vector<const Leaf*> leaves;

        while (true) {
          auto i = next_index++;
          if (i >= b.num_faces()) {
            break;
          }

          Face_handle b_fh{i};
          if (b_faces_to_ignore.contains(b_fh)) {
            continue;
          }

          leaves.clear();
          a_tree.get_intersecting_leaves(std::back_inserter(leaves), b.triangle(b_fh).bbox());

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
        }

        std::lock_guard lock{mutex};
        pairs.insert(pairs.end(), local_pairs.begin(), local_pairs.end());
      });
    }

    for (auto& thread : threads) {
      thread.join();
    }

    return pairs;
  }
};

}  // namespace kigumi
