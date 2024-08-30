#pragma once

#include <kigumi/io.h>

#include <cstdint>
#include <iostream>

namespace kigumi {

enum class Face_tag : std::uint8_t { Unknown = 0, Exterior, Interior, Coplanar, Opposite };

template <>
struct Write<Face_tag> {
  void operator()(std::ostream& out, const Face_tag& t) const {
    kigumi_write<std::uint8_t>(out, static_cast<std::uint8_t>(t));
  }
};

template <>
struct Read<Face_tag> {
  void operator()(std::istream& in, Face_tag& t) const {
    std::uint8_t x{};
    kigumi_read<std::uint8_t>(in, x);
    t = static_cast<Face_tag>(x);
  }
};

}  // namespace kigumi
