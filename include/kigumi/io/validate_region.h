#pragma once

#include <kigumi/Triangle_soup.h>

#include <iostream>

namespace kigumi::io {

template <class K, class FaceData>
bool validate_region(const Triangle_soup<K, FaceData>& soup, bool empty, bool full) {
  if (empty && full) {
    std::cerr << "region cannot be both empty and full" << std::endl;
    return false;
  }

  if (empty && soup.num_faces() != 0) {
    std::cerr << "empty region cannot have boundary faces" << std::endl;
    return false;
  }

  if (full && soup.num_faces() != 0) {
    std::cerr << "full region cannot have boundary faces" << std::endl;
    return false;
  }

  if (!empty && !full && soup.num_faces() == 0) {
    std::cerr << "region boundary must not be empty" << std::endl;
    return false;
  }

  return true;
}

}  // namespace kigumi::io
