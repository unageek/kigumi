#pragma once

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Mesh_entities.h>
#include <kigumi/Null_data.h>
#include <kigumi/Region.h>
#include <kigumi/Triangle_soup.h>

#include <utility>

template <class K, class FaceData = kigumi::Null_data>
kigumi::Region<K, FaceData> make_cube(const typename K::Point_3& min,
                                      const typename K::Point_3& max, const FaceData& data,
                                      bool invert = false) {
  auto make_face = [invert](const kigumi::Face& f) {
    return invert ? kigumi::Face{f[0], f[2], f[1]} : f;
  };

  kigumi::Triangle_soup<K, FaceData> soup;

  auto vi1 = soup.add_vertex({min.x(), min.y(), min.z()});
  auto vi2 = soup.add_vertex({max.x(), min.y(), min.z()});
  auto vi3 = soup.add_vertex({min.x(), max.y(), min.z()});
  auto vi4 = soup.add_vertex({max.x(), max.y(), min.z()});
  auto vi5 = soup.add_vertex({min.x(), min.y(), max.z()});
  auto vi6 = soup.add_vertex({max.x(), min.y(), max.z()});
  auto vi7 = soup.add_vertex({min.x(), max.y(), max.z()});
  auto vi8 = soup.add_vertex({max.x(), max.y(), max.z()});

  soup.add_face(make_face({vi1, vi2, vi6}));
  soup.add_face(make_face({vi1, vi3, vi4}));
  soup.add_face(make_face({vi1, vi4, vi2}));
  soup.add_face(make_face({vi1, vi5, vi7}));
  soup.add_face(make_face({vi1, vi6, vi5}));
  soup.add_face(make_face({vi1, vi7, vi3}));
  soup.add_face(make_face({vi8, vi2, vi4}));
  soup.add_face(make_face({vi8, vi3, vi7}));
  soup.add_face(make_face({vi8, vi4, vi3}));
  soup.add_face(make_face({vi8, vi5, vi6}));
  soup.add_face(make_face({vi8, vi6, vi2}));
  soup.add_face(make_face({vi8, vi7, vi5}));

  for (auto fi : soup.faces()) {
    soup.data(fi) = data;
  }

  return kigumi::Region<K, FaceData>{std::move(soup)};
}
