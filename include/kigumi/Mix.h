#pragma once

#include <kigumi/Classify_faces_globally.h>
#include <kigumi/Classify_faces_locally.h>
#include <kigumi/Corefine.h>
#include <kigumi/Find_border_edges.h>
#include <kigumi/Mixed.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Warnings.h>

#include <atomic>
#include <iostream>
#include <iterator>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Mix {
  using Classify_faces_globally = Classify_faces_globally<K, FaceData>;
  using Classify_faces_locally = Classify_faces_locally<K, FaceData>;
  using Find_border_edges = Find_border_edges<K, FaceData>;
  using Mixed_triangle_soup = Mixed_triangle_soup<K, FaceData>;
  using Triangle_soup = Triangle_soup<K, FaceData>;

 public:
  std::pair<Mixed_triangle_soup, Warnings> operator()(const Triangle_soup& left,
                                                      const Triangle_soup& right) const {
    Corefine corefine{left, right};

    std::cout << "Constructing mixed mesh..." << std::endl;

    Mixed_triangle_mesh<K, FaceData> m(corefine.point_list().into_vector());

    std::vector<std::array<std::size_t, 3>> tris;
    for (auto fh : left.faces()) {
      tris.clear();
      corefine.get_left_triangles(fh, std::back_inserter(tris));
      for (const auto& tri : tris) {
        auto new_fh =
            m.add_face({Vertex_handle{tri[0]}, Vertex_handle{tri[1]}, Vertex_handle{tri[2]}});
        m.data(new_fh).from_left = true;
        m.data(new_fh).data = left.data(fh);
      }
    }

    for (auto fh : right.faces()) {
      tris.clear();
      corefine.get_right_triangles(fh, std::back_inserter(tris));
      for (const auto& tri : tris) {
        auto new_fh =
            m.add_face({Vertex_handle{tri[0]}, Vertex_handle{tri[1]}, Vertex_handle{tri[2]}});
        m.data(new_fh).from_left = false;
        m.data(new_fh).data = right.data(fh);
      }
    }

    m.finalize();

    std::cout << "Local classification..." << std::endl;

    auto border_edges = Find_border_edges{}(m);
    Warnings warnings{};

    {
      std::vector<Edge> border_edges_vec(border_edges.begin(), border_edges.end());
      std::vector<std::thread> threads;
      std::atomic<std::size_t> next_index{};
      std::mutex mutex;
      auto num_threads = std::thread::hardware_concurrency();
      for (unsigned tid = 0; tid < num_threads; ++tid) {
        threads.emplace_back([&] {
          Classify_faces_locally classify_faces_locally;
          Warnings local_warnings{};

          while (true) {
            auto i = next_index++;
            if (i >= border_edges_vec.size()) {
              break;
            }

            const auto& edge = border_edges_vec.at(i);
            local_warnings |= classify_faces_locally(m, edge, border_edges);
          }

          std::lock_guard lock{mutex};
          warnings |= local_warnings;
        });
      }

      for (auto& thread : threads) {
        thread.join();
      }
    }

    std::cout << "Global classification..." << std::endl;

    Classify_faces_globally classify_globally;
    warnings |= classify_globally(m, border_edges, left, right);

    return {m.into_Triangle_soup(), warnings};
  }
};

}  // namespace kigumi
