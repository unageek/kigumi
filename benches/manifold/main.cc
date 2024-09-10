#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/number_utils.h>
#include <kigumi/Mesh_indices.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Triangle_soup_io.h>
#include <manifold.h>

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

bool read_manifold(const std::string& filename, manifold::Manifold& manifold) {
  Triangle_soup soup;
  if (!read_triangle_soup(filename, soup)) {
    return false;
  }

  manifold::Mesh mesh;
  for (auto vh : soup.vertices()) {
    const auto& p = soup.point(vh);
    mesh.vertPos.emplace_back(CGAL::to_double(p.x()), CGAL::to_double(p.y()),
                              CGAL::to_double(p.z()));
  }
  for (auto fh : soup.faces()) {
    const auto& f = soup.face(fh);
    mesh.triVerts.emplace_back(f[0].idx(), f[1].idx(), f[2].idx());
  }

  manifold = manifold::Manifold{mesh};
  return true;
}

bool write_manifold(const std::string& filename, const manifold::Manifold& manifold) {
  auto mesh = manifold.GetMesh();

  Triangle_soup soup;
  for (const auto& p : mesh.vertPos) {
    soup.add_vertex({p.x, p.y, p.z});
  }
  for (const auto& tri : mesh.triVerts) {
    soup.add_face({Vertex_index(tri.x), Vertex_index(tri.y), Vertex_index(tri.z)});
  }

  return write_triangle_soup(filename, soup);
}

}  // namespace

int main(int argc, char* argv[]) {
  try {
    std::vector<std::string> args(argv + 1, argv + argc);

    manifold::Manifold first;
    manifold::Manifold second;

    read_manifold(args.at(0), first);
    read_manifold(args.at(1), second);

    auto start = std::chrono::high_resolution_clock::now();
    auto result = first.Boolean(second, manifold::OpType::Intersect);
    result.Status();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;

    write_manifold(args.at(2), result);

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "unknown error" << std::endl;
    return 1;
  }
}
