#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/number_utils.h>
#include <geogram/basic/common.h>
#include <geogram/mesh/mesh.h>
#include <geogram/mesh/mesh_surface_intersection.h>
#include <kigumi/Mesh_indices.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Triangle_soup_io.h>

#include <array>
#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Triangle_soup = kigumi::Triangle_soup<K>;
using kigumi::read_triangle_soup;
using kigumi::Vertex_index;
using kigumi::write_triangle_soup;

namespace {

bool read_mesh(const std::string& filename, GEO::Mesh& mesh) {
  Triangle_soup soup;
  if (!read_triangle_soup(filename, soup)) {
    return false;
  }

  mesh.clear();
  for (auto vh : soup.vertices()) {
    const auto& p = soup.point(vh);
    std::array<double, 3> point{CGAL::to_double(p.x()), CGAL::to_double(p.y()),
                                CGAL::to_double(p.z())};
    mesh.vertices.create_vertex(point.data());
  }
  for (auto fh : soup.faces()) {
    const auto& f = soup.face(fh);
    mesh.facets.create_triangle(f[0].idx(), f[1].idx(), f[2].idx());
  }

  return true;
}

bool write_mesh(const std::string& filename, const GEO::Mesh& mesh) {
  Triangle_soup soup;
  for (GEO::index_t i = 0; i < mesh.vertices.nb(); ++i) {
    const auto& p = mesh.vertices.point(i);
    soup.add_vertex({p[0], p[1], p[2]});
  }
  for (GEO::index_t i = 0; i < mesh.facets.nb(); ++i) {
    if (mesh.facets.nb_vertices(i) != 3) {
      throw std::runtime_error("only triangle facets are supported");
    }
    auto v1 = mesh.facets.vertex(i, 0);
    auto v2 = mesh.facets.vertex(i, 1);
    auto v3 = mesh.facets.vertex(i, 2);
    soup.add_face({Vertex_index{v1}, Vertex_index{v2}, Vertex_index{v3}});
  }

  return write_triangle_soup(filename, soup);
}

}  // namespace

int main(int argc, char* argv[]) {
  try {
    GEO::initialize(GEO::GEOGRAM_INSTALL_ALL);

    std::vector<std::string> args(argv + 1, argv + argc);

    GEO::Mesh first;
    GEO::Mesh second;
    GEO::Mesh result;

    read_mesh(args.at(0), first);
    read_mesh(args.at(1), second);

    auto start = std::chrono::high_resolution_clock::now();
    GEO::mesh_intersection(result, first, second);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;

    write_mesh(args.at(2), result);

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "unknown error" << std::endl;
    return 1;
  }
}
