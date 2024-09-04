#pragma once

#include <kigumi/Face_tag.h>
#include <kigumi/Mesh_entities.h>
#include <kigumi/Mesh_handles.h>
#include <kigumi/Mixed.h>
#include <kigumi/Warnings.h>

#include <queue>
#include <stdexcept>
#include <unordered_set>

namespace kigumi {

template <class K, class FaceData>
class Propagate_face_tags {
  using Mixed_triangle_mesh = Mixed_triangle_mesh<K, FaceData>;

 public:
  Warnings operator()(Mixed_triangle_mesh& m, const std::unordered_set<Edge>& border_edges,
                      Face_handle seed) const {
    const auto& data = m.data(seed);
    auto from_left = data.from_left;
    auto tag = data.tag;

    if (tag != Face_tag::INTERIOR && tag != Face_tag::EXTERIOR) {
      throw std::runtime_error("the seed face must be tagged as either interior or exterior");
    }

    Warnings warnings{};

    queue_.push(seed);
    while (!queue_.empty()) {
      auto fh = queue_.front();
      queue_.pop();

      for (auto fh2 : m.faces_around_face(fh, border_edges)) {
        auto& tag2 = m.data(fh2).tag;
        if (tag2 == Face_tag::UNKNOWN) {
          tag2 = tag;
          queue_.push(fh2);
        } else if (tag2 != tag) {
          if (from_left) {
            warnings |= Warnings::FIRST_MESH_PARTIALLY_INTERSECTS_WITH_SECOND_MESH;
          } else {
            warnings |= Warnings::SECOND_MESH_PARTIALLY_INTERSECTS_WITH_FIRST_MESH;
          }
        }
      }
    }

    return warnings;
  }

 private:
  mutable std::queue<Face_handle> queue_;
};

}  // namespace kigumi
