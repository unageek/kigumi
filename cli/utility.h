#pragma once

#include <kigumi/Region.h>
#include <kigumi/Region_io.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/Triangle_soup_io.h>

#include <string>
#include <utility>

template <class K, class FaceData>
bool read_region(const std::string& filename, kigumi::Region<K, FaceData>& region) {
  using namespace kigumi;
  using Region = Region<K, FaceData>;
  using Triangle_soup = Triangle_soup<K, FaceData>;

  if (filename == ":empty:") {
    region = Region::empty();
    return true;
  }
  if (filename == ":full:") {
    region = Region::full();
    return true;
  }

  if (filename.ends_with(".kigumi")) {
    return read_kigumi_region(filename, region);
  }

  Triangle_soup boundary;
  if (read_triangle_soup(filename, boundary)) {
    region = Region{std::move(boundary)};
    return true;
  }

  return false;
}

template <class K, class FaceData>
bool write_region(const std::string& filename, const kigumi::Region<K, FaceData>& region) {
  using namespace kigumi;

  if (filename.ends_with(".kigumi")) {
    return write_kigumi_region(filename, region);
  }

  return write_triangle_soup(filename, region.boundary());
}
