#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Kigumi_mesh.h>
#include <kigumi/Warnings.h>
#include <kigumi/io.h>

#include <iostream>
#include <stdexcept>

#include "parse_options.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;

int main(int argc, const char* argv[]) {
  try {
    auto opts = parse_options(argc, argv);

    auto first = kigumi::Kigumi_mesh<K>::import(opts.first);
    if (first.is_empty()) {
      throw std::runtime_error("the first mesh is empty");
    }

    auto second = kigumi::Kigumi_mesh<K>::import(opts.second);
    if (second.is_empty()) {
      throw std::runtime_error("the second mesh is empty");
    }

    auto [boolean, warnings] = first.boolean(second);
    if ((warnings & kigumi::Warnings::FirstMeshPartiallyIntersectsWithSecondMesh) !=
        kigumi::Warnings::None) {
      std::cerr << "Warning: the first mesh partially intersects with the second mesh" << std::endl;
    }
    if ((warnings & kigumi::Warnings::SecondMeshPartiallyIntersectsWithFirstMesh) !=
        kigumi::Warnings::None) {
      std::cerr << "Warning: the second mesh partially intersects with the first mesh" << std::endl;
    }
    kigumi::save(opts.output_file, boolean);

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Error: unknown error" << std::endl;
    return 1;
  }
}
