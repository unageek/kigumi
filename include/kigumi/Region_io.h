#pragma once

#include <kigumi/Region.h>
#include <kigumi/io/kigumi.h>
#include <kigumi/io/obj.h>
#include <kigumi/io/off.h>
#include <kigumi/io/ply.h>

#include <iostream>
#include <string>

namespace kigumi {

template <class K, class FaceData>
bool read_region(const std::string& filename, Region<K, FaceData>& region) {
  if (filename.ends_with(".kigumi")) {
    return io::read_kigumi(filename, region);
  } else if (filename.ends_with(".obj")) {
    return io::read_obj(filename, region);
  } else if (filename.ends_with(".off")) {
    return io::read_off(filename, region);
  } else if (filename.ends_with(".ply")) {
    return io::read_ply(filename, region);
  } else {
    std::cerr << "unsupported file format" << std::endl;
    return false;
  }
  return true;
}

template <class K, class FaceData>
bool write_region(const std::string& filename, const Region<K, FaceData>& region) {
  if (filename.ends_with(".kigumi")) {
    return io::write_kigumi(filename, region);
  } else if (filename.ends_with(".obj")) {
    return io::write_obj(filename, region);
  } else if (filename.ends_with(".off")) {
    return io::write_off(filename, region);
  } else if (filename.ends_with(".ply")) {
    return io::write_ply(filename, region);
  } else {
    std::cerr << "unsupported file format" << std::endl;
    return false;
  }
  return true;
}

}  // namespace kigumi
