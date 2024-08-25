#pragma once

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Kigumi_mesh.h>
#include <kigumi/Mesh_entities.h>
#include <kigumi/Null_data.h>
#include <kigumi/Triangle_soup.h>

#include <utility>

template <class K, class FaceData = kigumi::Null_data>
kigumi::Kigumi_mesh<K, FaceData> make_cube(const typename K::Point_3& min,
                                           const typename K::Point_3& max, const FaceData& data,
                                           bool invert = false) {
  auto make_face = [invert](const kigumi::Face& f) {
    return invert ? kigumi::Face{f[0], f[2], f[1]} : f;
  };

  kigumi::Triangle_soup<K, FaceData> soup;

  auto vh1 = soup.add_vertex({min.x(), min.y(), min.z()});
  auto vh2 = soup.add_vertex({max.x(), min.y(), min.z()});
  auto vh3 = soup.add_vertex({min.x(), max.y(), min.z()});
  auto vh4 = soup.add_vertex({max.x(), max.y(), min.z()});
  auto vh5 = soup.add_vertex({min.x(), min.y(), max.z()});
  auto vh6 = soup.add_vertex({max.x(), min.y(), max.z()});
  auto vh7 = soup.add_vertex({min.x(), max.y(), max.z()});
  auto vh8 = soup.add_vertex({max.x(), max.y(), max.z()});

  soup.add_face(make_face({vh1, vh2, vh6}));
  soup.add_face(make_face({vh1, vh3, vh4}));
  soup.add_face(make_face({vh1, vh4, vh2}));
  soup.add_face(make_face({vh1, vh5, vh7}));
  soup.add_face(make_face({vh1, vh6, vh5}));
  soup.add_face(make_face({vh1, vh7, vh3}));
  soup.add_face(make_face({vh8, vh2, vh4}));
  soup.add_face(make_face({vh8, vh3, vh7}));
  soup.add_face(make_face({vh8, vh4, vh3}));
  soup.add_face(make_face({vh8, vh5, vh6}));
  soup.add_face(make_face({vh8, vh6, vh2}));
  soup.add_face(make_face({vh8, vh7, vh5}));

  for (auto fh : soup.faces()) {
    soup.data(fh) = data;
  }

  return kigumi::Kigumi_mesh<K, FaceData>{std::move(soup)};
}
