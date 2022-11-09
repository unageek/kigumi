#pragma once

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <boole/Mesh.h>

#include <limits>

namespace boole {

using K = CGAL::Exact_predicates_exact_constructions_kernel;

enum class Face_tag { Union, Intersection, Unknown, Deleted };

struct Face_data {
  bool from_left = false;
  Face_tag tag = Face_tag::Unknown;
  std::size_t connected_component_id = std::numeric_limits<std::size_t>::max();
};

using Mixed_mesh = Mesh<K, Face_data>;

}  // namespace boole
