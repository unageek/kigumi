#pragma once

#include <boole/Mixed_mesh.h>

#include <unordered_map>
#include <vector>

namespace boole {

inline Polygon_soup<K> extract(const Mixed_mesh& m, Face_tag tag) {
  std::vector<K::Point_3> points;
  std::vector<std::array<std::size_t, 3>> faces;
  std::unordered_map<Vertex_handle, std::size_t> map;

  for (auto fh : m.faces()) {
    if (m.data(fh).tag != tag) {
      continue;
    }

    const auto& f = m.face(fh);
    for (auto vh : f) {
      const auto& p = m.point(vh);
      if (!map.contains(vh)) {
        map.emplace(vh, points.size());
        points.push_back(p);
      }
    }
    faces.push_back({map.at(f[0]), map.at(f[1]), map.at(f[2])});
  }

  return {std::move(points), std::move(faces)};
}

}  // namespace boole
