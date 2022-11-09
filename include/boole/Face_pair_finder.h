#pragma once

#include <boole/AABB_tree/Overlap.h>
#include <boole/Polygon_soup.h>

#include <iterator>
#include <utility>
#include <vector>

namespace boole {

template <class Kernel>
class Face_pair_finder {
  using K = Kernel;
  using Leaf = typename Polygon_soup<K>::Leaf;
  using Face_pair = std::pair<std::size_t, std::size_t>;

 public:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  Face_pair_finder(const Polygon_soup<K>& left, const Polygon_soup<K>& right)
      : left_(left), right_(right) {}

  std::vector<Face_pair> find_face_pairs() const {
    std::vector<Face_pair> pairs;

#pragma omp parallel default(shared)
    {
      std::vector<Face_pair> local_pairs;
      std::vector<const Leaf*> leaves;

      if (left_.faces().size() < right_.faces().size()) {
        const auto& left_tree = left_.aabb_tree();

#pragma omp for schedule(guided)
        for (std::size_t i = 0; i < right_.faces().size(); ++i) {
          leaves.clear();
          left_tree.template get_intersecting_leaves<Overlap<K>>(std::back_inserter(leaves),
                                                                 right_.triangle(i));
          for (auto leaf : leaves) {
            local_pairs.emplace_back(leaf->face_index(), i);
          }
        }
      } else {
        const auto& right_tree = right_.aabb_tree();

#pragma omp for schedule(guided)
        for (std::size_t i = 0; i < left_.faces().size(); ++i) {
          leaves.clear();
          right_tree.template get_intersecting_leaves<Overlap<K>>(std::back_inserter(leaves),
                                                                  left_.triangle(i));
          for (auto leaf : leaves) {
            local_pairs.emplace_back(i, leaf->face_index());
          }
        }
      }

#pragma omp critical
      pairs.insert(pairs.end(), local_pairs.begin(), local_pairs.end());
    }

    return pairs;
  }

 private:
  const Polygon_soup<K>& left_;
  const Polygon_soup<K>& right_;
};

}  // namespace boole
