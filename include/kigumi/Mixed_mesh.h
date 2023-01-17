#pragma once

#include <kigumi/Mesh.h>
#include <kigumi/Polygon_soup.h>
#include <kigumi/io.h>

#include <array>
#include <fstream>
#include <stdexcept>

namespace kigumi {

enum class Face_tag { Unknown = 0, Union, Intersection, Coplanar, Opposite };

struct Face_data {
  bool from_left = false;
  Face_tag tag = Face_tag::Unknown;
};

template <class K>
using Mixed_mesh = Mesh<K, Face_data>;

template <class K>
using Mixed_polygon_soup = Polygon_soup<K, Face_data>;

template <class MixedMesh>
inline void write_mixed_mesh(const std::string& filename, const MixedMesh& m) {
  std::ofstream out(filename, std::ios::binary);
  if (!out) {
    throw std::runtime_error{"Failed to open file: " + filename};
  }

  my_write(out, m.num_vertices());
  my_write(out, m.num_faces());

  for (std::size_t i = 0; i < m.num_vertices(); ++i) {
    const auto& p = m.point({i});

    if (p.approx().x().is_point() && p.approx().y().is_point() && p.approx().z().is_point()) {
      my_write(out, char{0});
      my_write(out, p.approx().x().inf());
      my_write(out, p.approx().y().inf());
      my_write(out, p.approx().z().inf());
    } else {
      my_write<char>(out, char{1});
      my_write<mpq_class>(out, p.exact().x());
      my_write<mpq_class>(out, p.exact().y());
      my_write<mpq_class>(out, p.exact().z());
    }
  }

  for (const auto& fh : m.faces()) {
    const auto& f = m.face(fh);
    const auto& f_data = m.data(fh);
    my_write(out, f[0].i);
    my_write(out, f[1].i);
    my_write(out, f[2].i);
    my_write(out, static_cast<char>(f_data.from_left));
    my_write(out, static_cast<char>(f_data.tag));
  }
}

template <class MixedMesh>
inline MixedMesh read_mixed_mesh(const std::string& filename) {
  MixedMesh mesh{};

  std::ifstream in(filename, std::ios::binary);
  if (!in) {
    throw std::runtime_error{"Failed to open file: " + filename};
  }

  std::size_t num_vertices{};
  std::size_t num_faces{};

  my_read(in, num_vertices);
  my_read(in, num_faces);

  for (std::size_t i = 0; i < num_vertices; ++i) {
    char is_exact{};
    my_read(in, is_exact);

    if (!is_exact) {
      double x{};
      double y{};
      double z{};
      my_read(in, x);
      my_read(in, y);
      my_read(in, z);
      mesh.add_vertex({x, y, z});
    } else {
      mpq_class x{};
      mpq_class y{};
      mpq_class z{};
      my_read<mpq_class>(in, x);
      my_read<mpq_class>(in, y);
      my_read<mpq_class>(in, z);
      mesh.add_vertex({CGAL::Lazy_exact_nt<mpq_class>(x), CGAL::Lazy_exact_nt<mpq_class>(y),
                       CGAL::Lazy_exact_nt<mpq_class>(z)});
    }
  }

  for (std::size_t i = 0; i < num_faces; ++i) {
    std::array<std::size_t, 3> face{};
    bool from_left{};
    Face_tag tag = Face_tag::Unknown;
    char from_left_char{};
    char tag_char{};
    my_read(in, face[0]);
    my_read(in, face[1]);
    my_read(in, face[2]);
    my_read(in, from_left_char);
    my_read(in, tag_char);
    from_left = static_cast<bool>(from_left_char);
    tag = static_cast<Face_tag>(tag_char);
    auto fh =
        mesh.add_face({Vertex_handle{face[0]}, Vertex_handle{face[1]}, Vertex_handle{face[2]}});
    auto& f_data = mesh.data(fh);
    f_data.from_left = from_left;
    f_data.tag = tag;
  }

  return mesh;
}

}  // namespace kigumi
