#define _CRT_SECURE_NO_WARNINGS

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Boolean_operator.h>
#include <kigumi/Boolean_region_builder.h>
#include <kigumi/Region.h>
#include <kigumi/Triangle_soup_io.h>
#include <kigumi/threading.h>

#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "../cli/utility.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Region = kigumi::Region<K>;
using kigumi::Boolean_operator;
using kigumi::Boolean_region_builder;
using kigumi::read_triangle_soup;
using kigumi::Threading_context;
using kigumi::write_triangle_soup;

int main(int argc, char* argv[]) {
  try {
    const auto* env_num_threads = std::getenv("KIGUMI_NUM_THREADS");
    auto threading_opts = Threading_context::current();
    if (env_num_threads != nullptr) {
      threading_opts.set_num_threads(static_cast<std::size_t>(std::atoi(env_num_threads)));
    }
    Threading_context threading_ctx{threading_opts};
    std::cout << "num_threads: " << Threading_context::current().num_threads()
              << " (can be set with the environment variable KIGUMI_NUM_THREADS)" << std::endl;

    std::vector<std::string> args(argv + 1, argv + argc);

    Region first;
    Region second;

    read_region(args.at(0), first);
    read_region(args.at(1), second);

    auto start = std::chrono::high_resolution_clock::now();
    auto result = Boolean_region_builder{first, second}(Boolean_operator::INTERSECTION);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;

    write_region(args.at(2), result);

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "unknown error" << std::endl;
    return 1;
  }
}
