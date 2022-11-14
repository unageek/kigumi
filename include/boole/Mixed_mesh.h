#pragma once

#include <boole/Mesh.h>

#include <limits>

namespace boole {

enum class Face_tag { Union, Intersection, Unknown, Deleted };

struct Face_data {
  bool from_left = false;
  Face_tag tag = Face_tag::Unknown;
  std::size_t connected_component_id = std::numeric_limits<std::size_t>::max();
};

template <class K>
using Mixed_mesh = Mesh<K, Face_data>;

}  // namespace boole
