#pragma once

#include <boole/Mixed_mesh.h>

#include <queue>
#include <stdexcept>

namespace boole {

template <class K>
class Face_tag_propagator {
 public:
  explicit Face_tag_propagator(Mixed_mesh<K>& m) : m_(m) {
    for (auto fh : m_.faces()) {
      auto tag = m_.data(fh).tag;
      if (tag == Face_tag::Intersection || tag == Face_tag::Union) {
        queue_.push(fh);
      }
    }

    propagate();
  }

  Face_tag_propagator(Mixed_mesh<K>& m, Face_handle seed) : m_(m) {
    auto tag = m_.data(seed).tag;
    if (tag == Face_tag::Intersection || tag == Face_tag::Union) {
      queue_.push(seed);
    } else {
      throw std::runtime_error("seed face is not tagged as intersection or union");
    }

    propagate();
  }

 private:
  void propagate() {
    while (!queue_.empty()) {
      auto fh = queue_.front();
      queue_.pop();

      auto tag = m_.data(fh).tag;
      for (auto fh2 : m_.faces_around_face(fh)) {
        if (m_.data(fh2).tag == Face_tag::Unknown) {
          m_.data(fh2).tag = tag;
          queue_.push(fh2);
        }
      }
    }
  }

  Mixed_mesh<K>& m_;
  std::queue<Face_handle> queue_;
};

}  // namespace boole
