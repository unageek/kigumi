#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Mixed_mesh.h>
#include <kigumi/Polygon_soup.h>
#include <kigumi/boolean.h>

#include <iostream>
#include <string>
#include <vector>

#include "parse_options.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;

int main(int argc, const char* argv[]) {
  try {
    auto opts = parse_options(argc, argv);

    kigumi::Polygon_soup<K> first(opts.first);
    if (first.faces().empty()) {
      throw std::runtime_error("the first mesh is empty");
    }

    kigumi::Polygon_soup<K> second(opts.second);
    if (second.faces().empty()) {
      throw std::runtime_error("the second mesh is empty");
    }

    auto m = boolean(first, second);
    kigumi::save(opts.output_file, m);

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Error: unknown error" << std::endl;
    return 1;
  }
}
