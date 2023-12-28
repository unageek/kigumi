#pragma once

#include <kigumi/Mixed.h>
#include <kigumi/Warnings.h>

#include <queue>
#include <stdexcept>

namespace kigumi {

template <class K, class FaceData>
class Face_tag_propagator {
 public:
  Face_tag_propagator(Mixed_triangle_mesh<K, FaceData>& m, const std::unordered_set<Edge>& border,
                      Face_handle seed)
      : m_{m}, border_{border}, seed_{seed} {
    auto tag = m_.data(seed).tag;
    if (tag != Face_tag::Intersection && tag != Face_tag::Union) {
      throw std::runtime_error("seed face is not tagged as intersection or union");
    }

    propagate();
  }

  Warnings warnings() const { return warnings_; }

 private:
  void propagate() {
    const auto& data = m_.data(seed_);
    auto from_left = data.from_left;
    auto tag = data.tag;

    std::queue<Face_handle> queue_;
    queue_.push(seed_);

    while (!queue_.empty()) {
      auto fh = queue_.front();
      queue_.pop();

      for (auto fh2 : m_.faces_around_face(fh, border_)) {
        auto& tag2 = m_.data(fh2).tag;
        if (tag2 == Face_tag::Unknown) {
          tag2 = tag;
          queue_.push(fh2);
        } else if (tag2 != tag) {
          if (from_left) {
            warnings_ |= Warnings::FirstMeshPartiallyIntersectsWithSecondMesh;
          } else {
            warnings_ |= Warnings::SecondMeshPartiallyIntersectsWithFirstMesh;
          }
        }
      }
    }
  }

  Mixed_triangle_mesh<K, FaceData>& m_;
  const std::unordered_set<Edge>& border_;
  Face_handle seed_;
  Warnings warnings_{};
};

}  // namespace kigumi
