#pragma once

#include <boole/Mixed_mesh.h>

#include <limits>
#include <queue>
#include <vector>

namespace boole {

template <class K>
class Label_connected_components {
 public:
  explicit Label_connected_components(Mixed_mesh<K>& m) {
    std::queue<Face_handle> queue;
    auto from_left = false;
    auto connected_component_id = 0;
    auto begin = m.faces_begin();
    auto end = m.faces_end();

    while (true) {
      for (auto it = begin; it != end; ++it) {
        auto fh = *it;
        auto& f = m.data(fh);

        if (f.connected_component_id == std::numeric_limits<std::size_t>::max()) {
          begin = ++it;
          from_left = f.from_left;
          f.connected_component_id = connected_component_id;
          queue.push(fh);
          representative_faces_.push_back(fh);
          break;
        }
      }

      if (queue.empty()) {
        break;
      }

      while (!queue.empty()) {
        auto fh = queue.front();
        queue.pop();

        for (auto adj_fh : m.faces_around_face(fh)) {
          auto& adj_f = m.data(adj_fh);

          if (adj_f.from_left == from_left &&
              adj_f.connected_component_id == std::numeric_limits<std::size_t>::max()) {
            adj_f.connected_component_id = connected_component_id;
            queue.push(adj_fh);
          }
        }
      }

      ++connected_component_id;
    }
  }

  const std::vector<Face_handle>& representative_faces() const { return representative_faces_; }

 private:
  std::vector<Face_handle> representative_faces_;
};

}  // namespace boole
