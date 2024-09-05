#include <manifold/manifold.h>
#include <manifold/meshIO.h>

#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

using manifold::ExportMesh;
using manifold::ImportMesh;
using manifold::Manifold;

int main(int argc, char* argv[]) {
  try {
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
