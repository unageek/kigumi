#define _CRT_SECURE_NO_WARNINGS
#define TBB_PREVIEW_GLOBAL_CONTROL

#include <manifold/manifold.h>
#include <manifold/meshIO.h>
#include <tbb/global_control.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

using manifold::ExportMesh;
using manifold::ImportMesh;
using manifold::Manifold;

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
              << " (can be set by NUM_THREADS)" << std::endl;

    std::vector<std::string> args(argv + 1, argv + argc);

    Manifold first{ImportMesh(args.at(0))};
    Manifold second{ImportMesh(args.at(1))};

    auto start = std::chrono::high_resolution_clock::now();
    auto result = first.Boolean(second, manifold::OpType::Intersect);
    result.Status();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;

    ExportMesh(args.at(2), result.GetMesh(), {});

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "unknown error" << std::endl;
    return 1;
  }
}
