#define _CRT_SECURE_NO_WARNINGS
#define TBB_PREVIEW_GLOBAL_CONTROL

#include <geogram/basic/common.h>
#include <geogram/mesh/mesh.h>
#include <geogram/mesh/mesh_io.h>
#include <geogram/mesh/mesh_surface_intersection.h>
#include <tbb/global_control.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
  try {
    auto num_threads =
        tbb::global_control::active_value(tbb::global_control::max_allowed_parallelism);
    const auto* env_num_threads = std::getenv("NUM_THREADS");
    if (env_num_threads != nullptr) {
      num_threads = static_cast<std::size_t>(std::max(1, std::atoi(env_num_threads)));
    }
    tbb::global_control with_num_threads{tbb::global_control::max_allowed_parallelism, num_threads};
    std::cout << "num_threads: "
              << tbb::global_control::active_value(tbb::global_control::max_allowed_parallelism)
              << " (can be set with the environment variable NUM_THREADS)" << std::endl;

    GEO::initialize(GEO::GEOGRAM_INSTALL_ALL);

    std::vector<std::string> args(argv + 1, argv + argc);

    GEO::Mesh first;
    GEO::Mesh second;
    GEO::Mesh result;

    GEO::mesh_load(args.at(0), first);
    GEO::mesh_load(args.at(1), second);

    auto start = std::chrono::high_resolution_clock::now();
    GEO::mesh_intersection(result, first, second);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;

    GEO::mesh_save(result, args.at(2));

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "unknown error" << std::endl;
    return 1;
  }
}
