#pragma once

#include <boole/Mesh.h>

namespace boole {

enum class Face_tag { Union, Intersection, Unknown, Deleted };

struct Face_data {
  bool from_left = false;
  Face_tag tag = Face_tag::Unknown;
};

template <class K>
using Mixed_mesh = Mesh<K, Face_data>;

}  // namespace boole
