#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Boolean_operator.h>
#include <kigumi/Boolean_region_builder.h>
#include <kigumi/Global_options.h>
#include <kigumi/Region.h>
#include <kigumi/Region_io.h>

#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Face_data = int;
using Region = kigumi::Region<K, Face_data>;
using kigumi::Boolean_operator;
using kigumi::Boolean_region_builder;
using kigumi::Global_options;
using kigumi::read_region;
using kigumi::write_region;

int main(int argc, char* argv[]) {
  try {
    std::vector<std::string> args(argv + 1, argv + argc);

    const auto* env_num_threads = std::getenv("KIGUMI_NUM_THREADS");
    if (env_num_threads != nullptr) {
      auto num_threads = std::atoi(env_num_threads);
      Global_options::set_num_threads(num_threads);
    }
    std::cout << "num_threads: " << Global_options::num_threads()
              << " (can be set by KIGUMI_NUM_THREADS)" << std::endl;

    Region first;
    Region second;

    read_region(args.at(0), first);
    read_region(args.at(1), second);

    auto start = std::chrono::high_resolution_clock::now();
    auto result =
        Boolean_region_builder<K, Face_data>{first, second}(Boolean_operator::INTERSECTION);
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
