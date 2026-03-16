#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/number_utils.h>
#include <kigumi/Mesh_indices.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Triangle_soup_io.h>
#include <manifold/manifold.h>

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

  manifold::MeshGL64 mesh;
  for (auto vi : soup.vertices()) {
    const auto& p = soup.point(vi);
    mesh.vertProperties.push_back(CGAL::to_double(p.x()));
    mesh.vertProperties.push_back(CGAL::to_double(p.y()));
    mesh.vertProperties.push_back(CGAL::to_double(p.z()));
  }
  for (auto fi : soup.faces()) {
    const auto& f = soup.face(fi);
    mesh.triVerts.push_back(f[0].idx());
    mesh.triVerts.push_back(f[1].idx());
    mesh.triVerts.push_back(f[2].idx());
  }

  manifold = manifold::Manifold{mesh};
  return true;
}

bool write_manifold(const std::string& filename, const manifold::Manifold& manifold) {
  auto mesh = manifold.GetMeshGL64();

  Triangle_soup soup;
  for (std::size_t i = 0; i < mesh.vertProperties.size(); i += 3) {
    soup.add_vertex(
        {mesh.vertProperties.at(i), mesh.vertProperties.at(i + 1), mesh.vertProperties.at(i + 2)});
  }
  for (std::size_t i = 0; i < mesh.triVerts.size(); i += 3) {
    soup.add_face({Vertex_index(mesh.triVerts.at(i)), Vertex_index(mesh.triVerts.at(i + 1)),
                   Vertex_index(mesh.triVerts.at(i + 2))});
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
