#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <igl/copyleft/cgal/mesh_boolean.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Triangle_soup_io.h>

#include <Eigen/Core>
#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Triangle_soup = kigumi::Triangle_soup<K>;
using Faces = Eigen::Matrix<int, Eigen::Dynamic, 3>;
using Vertices = Eigen::Matrix<typename K::FT, Eigen::Dynamic, 3>;
using kigumi::read_triangle_soup;
using kigumi::Vertex_index;
using kigumi::write_triangle_soup;

namespace {

struct Mesh {
  Vertices vertices;
  Faces faces;
};

bool read_mesh(const std::string& filename, Mesh& mesh) {
  Triangle_soup soup;
  if (!read_triangle_soup(filename, soup)) {
    return false;
  }

  mesh.vertices.resize(soup.vertices().size(), 3);
  for (auto vi : soup.vertices()) {
    const auto& p = soup.point(vi);
    auto row = mesh.vertices.row(vi.idx());
    row << p.x(), p.y(), p.z();
  }
  mesh.faces.resize(soup.faces().size(), 3);
  for (auto fi : soup.faces()) {
    const auto& f = soup.face(fi);
    auto row = mesh.faces.row(fi.idx());
    row << f[0].idx(), f[1].idx(), f[2].idx();
  }

  return true;
}

bool write_mesh(const std::string& filename, const Mesh& mesh) {
  Triangle_soup soup;
  for (auto row : mesh.vertices.rowwise()) {
    soup.add_vertex({row(0), row(1), row(2)});
  }
  for (auto row : mesh.faces.rowwise()) {
    soup.add_face({Vertex_index{static_cast<std::size_t>(row(0))},
                   Vertex_index{static_cast<std::size_t>(row(1))},
                   Vertex_index{static_cast<std::size_t>(row(2))}});
  }

  return write_triangle_soup(filename, soup);
}

}  // namespace

int main(int argc, char* argv[]) {
  try {
    std::vector<std::string> args(argv + 1, argv + argc);

    Mesh first;
    Mesh second;
    Mesh result;

    read_mesh(args.at(0), first);
    read_mesh(args.at(1), second);

    auto start = std::chrono::high_resolution_clock::now();
    igl::copyleft::cgal::mesh_boolean(first.vertices, first.faces, second.vertices, second.faces,
                                      igl::MESH_BOOLEAN_TYPE_INTERSECT, result.vertices,
                                      result.faces);
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
