#pragma once

#include <kigumi/Corefine.h>
#include <kigumi/Global_face_classifier.h>
#include <kigumi/Local_face_classifier.h>
#include <kigumi/Mixed.h>
#include <kigumi/Shared_edge_finder.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Warnings.h>

#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Mixer {
 public:
  Mixer(const Triangle_soup<K, FaceData>& left, const Triangle_soup<K, FaceData>& right)
      : left_{left}, right_{right} {
    mix();
  }

  const Mixed_triangle_soup<K, FaceData>& mixed() const { return mixed_; }

  Mixed_triangle_soup<K, FaceData>&& into_mixed() { return std::move(mixed_); }

  Warnings warnings() const { return warnings_; }

 private:
  void mix() {
    Corefine corefine{left_, right_};

    std::cout << "Constructing mixed mesh..." << std::endl;

    Mixed_triangle_mesh<K, FaceData> m(corefine.point_list().into_vector());

    std::vector<std::array<std::size_t, 3>> tris;
    for (auto fh : left_.faces()) {
      tris.clear();
      corefine.get_left_triangles(fh, std::back_inserter(tris));
      for (const auto& tri : tris) {
        auto new_fh =
            m.add_face({Vertex_handle{tri[0]}, Vertex_handle{tri[1]}, Vertex_handle{tri[2]}});
        m.data(new_fh).from_left = true;
        m.data(new_fh).data = left_.data(fh);
      }
    }

    for (auto fh : right_.faces()) {
      tris.clear();
      corefine.get_right_triangles(fh, std::back_inserter(tris));
      for (const auto& tri : tris) {
        auto new_fh =
            m.add_face({Vertex_handle{tri[0]}, Vertex_handle{tri[1]}, Vertex_handle{tri[2]}});
        m.data(new_fh).from_left = false;
        m.data(new_fh).data = right_.data(fh);
      }
    }

    m.finalize();

    std::cout << "Local classification..." << std::endl;

    Shared_edge_finder shared_edge_finder{m};
    const auto& shared_edges = shared_edge_finder.shared_edges();

    {
      std::vector<Edge> shared_edges_vec(shared_edges.begin(), shared_edges.end());

#pragma omp parallel
      {
        Warnings local_warnings{};

#pragma omp for schedule(guided)
        // NOLINTNEXTLINE(modernize-loop-convert)
        for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(shared_edges_vec.size()); ++i) {
          Local_face_classifier lfc(m, shared_edges_vec.at(i), shared_edges);
          local_warnings |= lfc.warnings();
        }

#pragma omp critical
        warnings_ |= local_warnings;
      }
    }

    std::cout << "Global classification..." << std::endl;

    Global_face_classifier{m, shared_edges, left_, right_};

    mixed_ = m.into_Triangle_soup();
  }

  const Triangle_soup<K, FaceData>& left_;
  const Triangle_soup<K, FaceData>& right_;
  Mixed_triangle_soup<K, FaceData> mixed_;
  Warnings warnings_{};
};

}  // namespace kigumi
