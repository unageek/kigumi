#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Mixed_mesh.h>
#include <kigumi/extract.h>

#include <iostream>

#include "parse_options.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;

int main(int argc, const char* argv[]) {
  try {
    auto opts = parse_options(argc, argv);

    kigumi::Mixed_polygon_soup<K> m;
    kigumi::load(opts.input_file, m);

    auto result =
        kigumi::extract(m, opts.op, opts.extract_first, opts.extract_second, opts.prefer_first);
    result.save(opts.output_file);

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Error: unknown error" << std::endl;
    return 1;
  }
}
