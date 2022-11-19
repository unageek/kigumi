#pragma once

#include <boole/Mixed_mesh.h>

#include <queue>
#include <unordered_set>
#include <vector>

namespace boole {

template <class K>
class Connected_component_finder {
 public:
  explicit Connected_component_finder(Mixed_mesh<K>& m, const std::unordered_set<Edge>& border) {
    std::vector<bool> visited(m.num_faces(), false);
    std::queue<Face_handle> queue;
    auto begin = m.faces_begin();
    auto end = m.faces_end();

    while (true) {
      for (auto it = begin; it != end; ++it) {
        auto fh = *it;
        if (!visited.at(fh.i)) {
          visited.at(fh.i) = true;
          queue.push(fh);
          representative_faces_.push_back(fh);
          begin = ++it;
          break;
        }
      }

      if (queue.empty()) {
        break;
      }

      while (!queue.empty()) {
        auto fh = queue.front();
        queue.pop();

        for (auto adj_fh : m.faces_around_face(fh, border)) {
          if (!visited.at(adj_fh.i)) {
            visited.at(adj_fh.i) = true;
            queue.push(adj_fh);
          }
        }
      }
    }
  }

  const std::vector<Face_handle>& representative_faces() const { return representative_faces_; }

 private:
  std::vector<Face_handle> representative_faces_;
};

}  // namespace boole
