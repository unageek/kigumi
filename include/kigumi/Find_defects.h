#pragma once

#include <kigumi/Dense_undirected_graph.h>
#include <kigumi/Face_face_intersection.h>
#include <kigumi/Mesh_entities.h>
#include <kigumi/Mesh_indices.h>
#include <kigumi/Point_list.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/mesh_utility.h>
#include <kigumi/parallel_do.h>

#include <algorithm>
#include <array>
#include <boost/variant/get.hpp>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace kigumi {

template <class K, class FaceData>
class Find_defects {
  using Halfedge = std::array<Vertex_index, 2>;
  using Point_list = Point_list<K>;
  using Triangle_soup = Triangle_soup<K, FaceData>;
  using Leaf = typename Triangle_soup::Leaf;

 public:
  explicit Find_defects(const Triangle_soup& m)
      : m_{merge_duplicate_vertices(m)},
        isolated_vertices_{isolated_vertices(m_)},
        non_manifold_vertices_{non_manifold_vertices(m_)},
        halfedges_{collect_halfedges(m_)},
        boundary_edges_{boundary_edges(halfedges_)},
        inconsistent_edges_{inconsistent_edges(halfedges_)},
        non_manifold_edges_{non_manifold_edges(halfedges_)},
        trivial_degenerate_faces_{trivial_degenerate_faces(m_)},
        non_trivial_degenerate_faces_{non_trivial_degenerate_faces(m_)},
        overlapping_faces_{
            overlapping_faces(m_, trivial_degenerate_faces_, non_trivial_degenerate_faces_)} {}

  const std::vector<Edge>& boundary_edges() const { return boundary_edges_; }

  const std::vector<Edge>& inconsistent_edges() const { return inconsistent_edges_; }

  const std::vector<Vertex_index>& isolated_vertices() const { return isolated_vertices_; }

  const std::vector<Edge>& non_manifold_edges() const { return non_manifold_edges_; }

  const std::vector<Vertex_index>& non_manifold_vertices() const { return non_manifold_vertices_; }

  const std::vector<Face_index>& non_trivial_degenerate_faces() const {
    return non_trivial_degenerate_faces_;
  }

  const std::vector<Face_index>& overlapping_faces() const { return overlapping_faces_; }

  const Triangle_soup& triangle_soup() const { return m_; }

  const std::vector<Face_index>& trivial_degenerate_faces() const {
    return trivial_degenerate_faces_;
  }

 private:
  static Triangle_soup merge_duplicate_vertices(const Triangle_soup& m) {
    Triangle_soup new_m;

    std::vector<Vertex_index> vh_map;
    vh_map.reserve(m.num_vertices());

    Point_list points;
    points.start_uniqueness_check();
    for (auto vh : m.vertices()) {
      const auto& p = m.point(vh);
      auto next_idx = points.size();
      auto idx = points.insert(p);
      vh_map.push_back(Vertex_index{idx});
      if (idx == next_idx) {
        new_m.add_vertex(p);
      }
    }

    for (auto fh : m.faces()) {
      const auto& f = m.face(fh);
      Face new_f{vh_map.at(f[0].idx()), vh_map.at(f[1].idx()), vh_map.at(f[2].idx())};
      new_m.add_face(new_f);
    }

    return new_m;
  }

  static std::vector<Vertex_index> isolated_vertices(const Triangle_soup& m) {
    std::vector<Vertex_index> vhs;

    std::vector<bool> used(m.num_vertices(), false);
    for (auto fh : m.faces()) {
      for (auto vh : m.face(fh)) {
        used.at(vh.idx()) = true;
      }
    }

    for (auto vh : m.vertices()) {
      if (!used.at(vh.idx())) {
        vhs.emplace_back(vh);
      }
    }

    return vhs;
  }

  static std::vector<Vertex_index> non_manifold_vertices(const Triangle_soup& m) {
    std::vector<Vertex_index> vhs;

    std::vector<std::vector<Face_index>> vf_map(m.num_vertices());
    for (auto fh : m.faces()) {
      const auto& f = m.face(fh);
      if (f[0] == f[1] || f[1] == f[2] || f[2] == f[0]) {
        continue;
      }
      vf_map.at(f[0].idx()).push_back(fh);
      vf_map.at(f[1].idx()).push_back(fh);
      vf_map.at(f[2].idx()).push_back(fh);
    }

    auto next_vertex = [&](Face_index fh, Vertex_index vh) -> Vertex_index {
      const auto& f = m.face(fh);
      if (f[0] == vh) {
        return f[1];
      }
      if (f[1] == vh) {
        return f[2];
      }
      return f[0];
    };

    auto prev_vertex = [&](Face_index fh, Vertex_index vh) -> Vertex_index {
      const auto& f = m.face(fh);
      if (f[0] == vh) {
        return f[2];
      }
      if (f[1] == vh) {
        return f[0];
      }
      return f[1];
    };

    for (auto vh : m.vertices()) {
      const auto& v_fhs = vf_map.at(vh.idx());
      if (v_fhs.empty()) {
        // An isolated vertex.
        continue;
      }

      std::unordered_map<Vertex_index, std::size_t> local_vis;
      for (auto v_fh : v_fhs) {
        local_vis.emplace(next_vertex(v_fh, vh), local_vis.size());
        local_vis.emplace(prev_vertex(v_fh, vh), local_vis.size());
      }

      auto order = local_vis.size();
      dense_undirected_graph g{order};

      for (auto v_fh : v_fhs) {
        auto i = local_vis.at(next_vertex(v_fh, vh));
        auto j = local_vis.at(prev_vertex(v_fh, vh));
        g.add_edge(i, j);
      }

      if (!g.is_simple() || !g.is_connected() || g.max_degree() > 2) {
        vhs.push_back(vh);
      }
    }

    return vhs;
  }

  static Edge to_edge(const Halfedge& he) { return make_edge(he[0], he[1]); }

  static Halfedge opposite(const Halfedge& he) { return {he[1], he[0]}; }

  static std::unordered_multiset<Halfedge> collect_halfedges(const Triangle_soup& m) {
    std::unordered_multiset<Halfedge> hes;
    for (auto fh : m.faces()) {
      const auto& f = m.face(fh);
      if (f[0] == f[1] || f[1] == f[2] || f[2] == f[0]) {
        continue;
      }
      for (std::size_t i = 0; i < 3; ++i) {
        auto he = Halfedge{f.at(i), f.at((i + 1) % 3)};
        hes.insert(he);
      }
    }
    return hes;
  }

  static std::vector<Edge> boundary_edges(const std::unordered_multiset<Halfedge>& hes) {
    std::vector<Edge> edges;

    Halfedge last_he;
    for (const auto& he : hes) {
      if (he == last_he) {
        continue;
      }
      last_he = he;

      if (hes.count(he) == 1 && !hes.contains(opposite(he))) {
        edges.push_back(to_edge(he));
      }
    }

    return edges;
  }

  static std::vector<Edge> inconsistent_edges(const std::unordered_multiset<Halfedge>& hes) {
    std::vector<Edge> edges;

    Halfedge last_he;
    for (const auto& he : hes) {
      if (he == last_he) {
        continue;
      }
      last_he = he;

      if (hes.count(he) == 2 && !hes.contains(opposite(he))) {
        edges.push_back(to_edge(he));
      }
    }

    return edges;
  }

  static std::vector<Edge> non_manifold_edges(const std::unordered_multiset<Halfedge>& hes) {
    std::vector<Edge> edges;

    Halfedge last_he;
    for (const auto& he : hes) {
      if (he == last_he) {
        continue;
      }
      last_he = he;

      if (he[0] > he[1] && hes.contains(opposite(he))) {
        // Prevent double counting.
        continue;
      }

      if (hes.count(he) + hes.count(opposite(he)) > 2) {
        edges.push_back(to_edge(he));
      }
    }

    return edges;
  }

  static std::vector<Face_index> trivial_degenerate_faces(const Triangle_soup& m) {
    std::vector<Face_index> fhs;

    for (auto fh : m.faces()) {
      const auto& f = m.face(fh);
      if (f[0] == f[1] || f[1] == f[2] || f[2] == f[0]) {
        fhs.push_back(fh);
      }
    }

    return fhs;
  }

  static std::vector<Face_index> non_trivial_degenerate_faces(const Triangle_soup& m) {
    std::vector<Face_index> fhs;

    parallel_do(
        m.faces_begin(), m.faces_end(), std::vector<Face_index>{},
        [&](auto fh, auto& local_fhs) {
          const auto& f = m.face(fh);
          if (f[0] == f[1] || f[1] == f[2] || f[2] == f[0]) {
            return;
          }
          auto tri = m.triangle(fh);
          if (tri.is_degenerate()) {
            local_fhs.push_back(fh);
          }
        },
        [&](auto& local_fhs) {
          if (fhs.empty()) {
            fhs = std::move(local_fhs);
          } else {
            fhs.insert(fhs.end(), local_fhs.begin(), local_fhs.end());
          }
        });

    return fhs;
  }

  static std::vector<Face_index> overlapping_faces(
      const Triangle_soup& m, const std::vector<Face_index>& trivial_degenerate_faces,
      const std::vector<Face_index>& non_trivial_degenerate_faces) {
    std::vector<Face_index> fhs;

    std::unordered_set<Face_index> degenerate_faces;
    degenerate_faces.reserve(trivial_degenerate_faces.size() + non_trivial_degenerate_faces.size());
    degenerate_faces.insert(trivial_degenerate_faces.begin(), trivial_degenerate_faces.end());
    degenerate_faces.insert(non_trivial_degenerate_faces.begin(),
                            non_trivial_degenerate_faces.end());

    Point_list points;
    for (auto vh : m.vertices()) {
      points.insert(m.point(vh));
    }

    const auto& tree = m.aabb_tree();

    parallel_do(
        m.faces_begin(), m.faces_end(), std::vector<Face_index>{},
        [&](auto fh, auto& local_fhs) {
          thread_local Face_face_intersection face_face_intersection{points};
          thread_local std::vector<const Leaf*> leaves;
          thread_local std::vector<Vertex_index> shared_vertices;

          if (degenerate_faces.contains(fh)) {
            return;
          }

          leaves.clear();
          tree.get_intersecting_leaves(std::back_inserter(leaves), internal::face_bbox(m, fh));

          auto f = m.face(fh);
          std::sort(f.begin(), f.end());

          for (const auto* leaf : leaves) {
            auto fh2 = leaf->face_index();
            if (fh2 <= fh || degenerate_faces.contains(fh2)) {
              continue;
            }

            auto f2 = m.face(fh2);
            std::sort(f2.begin(), f2.end());

            auto inter = face_face_intersection(f[0].idx(), f[1].idx(), f[2].idx(), f2[0].idx(),
                                                f2[1].idx(), f2[2].idx());
            if (inter.empty()) {
              continue;
            }

            shared_vertices.clear();
            std::set_intersection(f.begin(), f.end(), f2.begin(), f2.end(),
                                  std::back_inserter(shared_vertices));
            auto num_shared_vertices = shared_vertices.size();

            if (inter.size() == 1) {
              if (num_shared_vertices < 1) {
                local_fhs.push_back(fh);
                return;
              }
            } else if (inter.size() == 2) {
              if (num_shared_vertices < 2) {
                local_fhs.push_back(fh);
                return;
              }
            } else {
              local_fhs.push_back(fh);
              return;
            }
          }
        },
        [&](auto& local_fhs) {
          if (fhs.empty()) {
            fhs = std::move(local_fhs);
          } else {
            fhs.insert(fhs.end(), local_fhs.begin(), local_fhs.end());
          }
        });

    return fhs;
  }

  Triangle_soup m_;
  std::vector<Vertex_index> isolated_vertices_;
  std::vector<Vertex_index> non_manifold_vertices_;
  std::unordered_multiset<Halfedge> halfedges_;
  std::vector<Edge> boundary_edges_;
  std::vector<Edge> inconsistent_edges_;
  std::vector<Edge> non_manifold_edges_;
  std::vector<Face_index> trivial_degenerate_faces_;
  std::vector<Face_index> non_trivial_degenerate_faces_;
  std::vector<Face_index> overlapping_faces_;
};

}  // namespace kigumi
