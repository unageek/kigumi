#pragma once

#include <kigumi/io.h>

namespace kigumi {

// Used as a substitute for std::nullptr_t to avoid a build error with MSVC.
struct Null_data {};

template <>
struct Write<Null_data> {
  static void write(std::ostream& /*out*/, const Null_data& /*tt*/) {
    // no-op
  }
};

template <>
struct Read<Null_data> {
  static void read(std::istream& /*in*/, Null_data& /*tt*/) {
    // no-op
  }
};

}  // namespace kigumi
