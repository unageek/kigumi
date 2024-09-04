#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/IO/io.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Surface_mesh.h>

#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace PMP = CGAL::Polygon_mesh_processing;
using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Surface_mesh = CGAL::Surface_mesh<K::Point_3>;

int main(int argc, char* argv[]) {
  try {
    std::vector<std::string> args(argv + 1, argv + argc);

    Surface_mesh first;
    Surface_mesh second;
    Surface_mesh result;

    CGAL::IO::read_polygon_mesh(args.at(0), first);
    CGAL::IO::read_polygon_mesh(args.at(1), second);

    auto start = std::chrono::high_resolution_clock::now();
    PMP::corefine_and_compute_intersection(first, second, result);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;

    CGAL::IO::write_polygon_mesh(args.at(2), result);

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "unknown error" << std::endl;
    return 1;
  }
}
