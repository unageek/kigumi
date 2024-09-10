#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
#include <CGAL/boost/graph/iterator.h>
#include <kigumi/Mesh_indices.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Triangle_soup_io.h>

#include <array>
#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace PMP = CGAL::Polygon_mesh_processing;
using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Surface_mesh = CGAL::Surface_mesh<K::Point_3>;
using SM_Vertex_index = Surface_mesh::Vertex_index;
using Triangle_soup = kigumi::Triangle_soup<K>;
using kigumi::read_triangle_soup;
using kigumi::Vertex_index;
using kigumi::write_triangle_soup;

namespace {

bool read_surface_mesh(const std::string& filename, Surface_mesh& mesh) {
  Triangle_soup soup;
  if (!read_triangle_soup(filename, soup)) {
    return false;
  }

  mesh.clear();
  for (auto vh : soup.vertices()) {
    const auto& p = soup.point(vh);
    mesh.add_vertex(p);
  }
  for (auto fh : soup.faces()) {
    const auto& f = soup.face(fh);
    auto v1 = SM_Vertex_index{static_cast<Surface_mesh::size_type>(f[0].idx())};
    auto v2 = SM_Vertex_index{static_cast<Surface_mesh::size_type>(f[1].idx())};
    auto v3 = SM_Vertex_index{static_cast<Surface_mesh::size_type>(f[2].idx())};
    mesh.add_face(v1, v2, v3);
  }

  return true;
}

bool write_surface_mesh(const std::string& filename, const Surface_mesh& mesh) {
  Triangle_soup soup;
  for (auto v : mesh.vertices()) {
    const auto& p = mesh.point(v);
    soup.add_vertex(p);
  }
  for (auto f : mesh.faces()) {
    std::array<Vertex_index, 3> face;
    auto* it = face.begin();
    for (auto v : CGAL::vertices_around_face(CGAL::halfedge(f, mesh), mesh)) {
      *it++ = Vertex_index{v};
    }
    soup.add_face({face});
  }

  return write_triangle_soup(filename, soup);
}

}  // namespace

int main(int argc, char* argv[]) {
  try {
    std::vector<std::string> args(argv + 1, argv + argc);

    Surface_mesh first;
    Surface_mesh second;
    Surface_mesh result;

    read_surface_mesh(args.at(0), first);
    read_surface_mesh(args.at(1), second);

    auto start = std::chrono::high_resolution_clock::now();
    PMP::corefine_and_compute_intersection(first, second, result);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;

    write_surface_mesh(args.at(2), result);

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "unknown error" << std::endl;
    return 1;
  }
}
