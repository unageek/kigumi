#define _CRT_SECURE_NO_WARNINGS

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Boolean_operator.h>
#include <kigumi/Boolean_region_builder.h>
#include <kigumi/Region.h>
#include <kigumi/Region_io.h>
#include <kigumi/contexts.h>

#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Region = kigumi::Region<K>;
using kigumi::Boolean_operator;
using kigumi::Boolean_region_builder;
using kigumi::Num_threads;
using kigumi::read_region;
using kigumi::write_region;

int main(int argc, char* argv[]) {
  try {
    const auto* env_num_threads = std::getenv("NUM_THREADS");
    std::size_t num_threads = Num_threads::current();
    if (env_num_threads != nullptr) {
      num_threads = static_cast<std::size_t>(std::atoi(env_num_threads));
    }
    Num_threads with_num_threads{num_threads};
    std::cout << "num_threads: " << Num_threads::current() << " (can be set by NUM_THREADS)"
              << std::endl;

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
