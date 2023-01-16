#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Mixed_mesh.h>
#include <kigumi/Polygon_soup.h>
#include <kigumi/boolean.h>

#include <iostream>
#include <string>
#include <vector>

using K = CGAL::Exact_predicates_exact_constructions_kernel;

int main(int argc, const char* argv[]) {
  try {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::vector<std::string> args(argv, argv + argc);
    if (args.size() != 4) {
      std::cerr << "Usage: " << args[0] << " <mesh1> <mesh2> <out>" << std::endl;
      return 1;
    }

    kigumi::Polygon_soup<K> left(args.at(1));
    if (left.faces().empty()) {
      throw std::runtime_error("the first mesh is empty");
    }

    kigumi::Polygon_soup<K> right(args.at(2));
    if (right.faces().empty()) {
      throw std::runtime_error("the second mesh is empty");
    }

    auto m = boolean(left, right);
    kigumi::write_mixed_mesh(args.at(3), m);

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Error: unknown error" << std::endl;
    return 1;
  }
}
