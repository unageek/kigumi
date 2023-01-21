#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Kigumi_mesh.h>
#include <kigumi/io.h>

#include <iostream>
#include <stdexcept>

#include "parse_options.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;

int main(int argc, const char* argv[]) {
  try {
    auto opts = parse_options(argc, argv);

    kigumi::Boolean_operation<K> boolean;
    kigumi::load(opts.input_file, boolean);

    auto result =
        boolean.apply(opts.op, opts.extract_first, opts.extract_second, opts.prefer_first);
    result.save_lossy(opts.output_file);

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Error: unknown error" << std::endl;
    return 1;
  }
}
