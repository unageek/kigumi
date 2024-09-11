#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/number_utils.h>
#include <kigumi/Mesh_indices.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Triangle_soup_io.h>
#include <mcut/mcut.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Triangle_soup = kigumi::Triangle_soup<K>;
using kigumi::read_triangle_soup;
using kigumi::Vertex_index;
using kigumi::write_triangle_soup;

namespace {

struct Mesh {
  std::vector<double> vertices;
  std::vector<McUint32> vertex_indices;
  std::vector<McUint32> face_sizes;
  McUint32 num_vertices;
  McUint32 num_faces;
};

bool read_mesh(const std::string& filename, Mesh& mesh) {
  Triangle_soup soup;
  if (!read_triangle_soup(filename, soup)) {
    return false;
  }

  for (auto vi : soup.vertices()) {
    const auto& p = soup.point(vi);
    mesh.vertices.push_back(CGAL::to_double(p.x()));
    mesh.vertices.push_back(CGAL::to_double(p.y()));
    mesh.vertices.push_back(CGAL::to_double(p.z()));
  }
  for (auto fi : soup.faces()) {
    const auto& f = soup.face(fi);
    mesh.vertex_indices.push_back(f[0].idx());
    mesh.vertex_indices.push_back(f[1].idx());
    mesh.vertex_indices.push_back(f[2].idx());
    mesh.face_sizes.push_back(3);
  }
  mesh.num_vertices = soup.num_vertices();
  mesh.num_faces = soup.num_faces();

  return true;
}

bool write_mesh(const std::string& filename, const Mesh& mesh) {
  Triangle_soup soup;
  for (McUint32 i = 0; i < mesh.num_vertices; ++i) {
    soup.add_vertex(
        {mesh.vertices.at(3 * i), mesh.vertices.at(3 * i + 1), mesh.vertices.at(3 * i + 2)});
  }
  for (McUint32 i = 0; i < mesh.num_faces; ++i) {
    soup.add_face({Vertex_index(mesh.vertex_indices.at(3 * i)),
                   Vertex_index(mesh.vertex_indices.at(3 * i + 1)),
                   Vertex_index(mesh.vertex_indices.at(3 * i + 2))});
  }

  return write_triangle_soup(filename, soup);
}

void check(McResult result) {
  if (result != MC_NO_ERROR) {
    throw std::runtime_error("mcut failed");
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  try {
    std::vector<std::string> args(argv + 1, argv + argc);

    Mesh first;
    Mesh second;

    read_mesh(args.at(0), first);
    read_mesh(args.at(1), second);

    McContext context = MC_NULL_HANDLE;
    check(mcCreateContext(&context, MC_NULL_HANDLE));

    McFlags dispatch_flags =
        MC_DISPATCH_VERTEX_ARRAY_DOUBLE | MC_DISPATCH_ENFORCE_GENERAL_POSITION |
        MC_DISPATCH_FILTER_FRAGMENT_SEALING_INSIDE | MC_DISPATCH_FILTER_FRAGMENT_LOCATION_BELOW;

    auto start = std::chrono::high_resolution_clock::now();
    check(mcDispatch(context, dispatch_flags, first.vertices.data(), first.vertex_indices.data(),
                     first.face_sizes.data(), first.num_vertices, first.num_faces,
                     second.vertices.data(), second.vertex_indices.data(), second.face_sizes.data(),
                     second.num_vertices, second.num_faces));
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;

    McUint32 num_ccs = 0;
    McConnectedComponentType cc_types = static_cast<McConnectedComponentType>(
        MC_CONNECTED_COMPONENT_TYPE_FRAGMENT | MC_CONNECTED_COMPONENT_TYPE_PATCH);
    check(mcGetConnectedComponents(context, cc_types, 0, nullptr, &num_ccs));
    std::vector<McConnectedComponent> ccs(num_ccs, MC_NULL_HANDLE);
    check(mcGetConnectedComponents(context, cc_types, num_ccs, ccs.data(), nullptr));

    Mesh result;

    for (McUint32 i = 0; i < num_ccs; ++i) {
      McConnectedComponent cc = ccs.at(i);
      McSize num_bytes = 0;

      check(mcGetConnectedComponentData(context, cc, MC_CONNECTED_COMPONENT_DATA_VERTEX_DOUBLE, 0,
                                        nullptr, &num_bytes));
      McUint32 num_vertices = num_bytes / (3 * sizeof(double));
      result.vertices.resize(3 * (result.num_vertices + num_vertices));
      check(mcGetConnectedComponentData(context, cc, MC_CONNECTED_COMPONENT_DATA_VERTEX_DOUBLE,
                                        num_bytes, result.vertices.data() + 3 * result.num_vertices,
                                        nullptr));

      check(mcGetConnectedComponentData(context, cc, MC_CONNECTED_COMPONENT_DATA_FACE_TRIANGULATION,
                                        0, nullptr, &num_bytes));
      McUint32 num_faces = num_bytes / (3 * sizeof(McUint32));
      result.vertex_indices.resize(3 * (result.num_faces + num_faces));
      check(mcGetConnectedComponentData(
          context, cc, MC_CONNECTED_COMPONENT_DATA_FACE_TRIANGULATION, num_bytes,
          result.vertex_indices.data() + 3 * result.num_faces, nullptr));
      std::transform(result.vertex_indices.begin() + 3 * result.num_faces,
                     result.vertex_indices.end(),
                     result.vertex_indices.begin() + 3 * result.num_faces,
                     [offset = result.num_vertices](McUint32 idx) { return offset + idx; });

      result.num_vertices += num_vertices;
      result.num_faces += num_faces;
    }

    result.face_sizes.resize(result.num_faces, 3);

    mcReleaseConnectedComponents(context, 0, nullptr);
    mcReleaseContext(context);

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
