#pragma once

#include <fast_float/fast_float.h>

#include <array>
#include <charconv>
#include <iostream>
#include <limits>
#include <string>
#include <system_error>

namespace kigumi::io::ascii {

struct Double {
  double value{};
};

inline std::istream& operator>>(std::istream& in, Double& d) {
  thread_local std::string s;

  if (!(in >> s)) {
    return in;
  }
  const auto* first = s.front() == '+' ? s.data() + 1 : s.data();
  const auto* last = s.data() + s.size();
  auto [ptr, ec] = fast_float::from_chars(first, last, d.value);
  if (ptr != last || ec != std::errc{}) {
    in.setstate(std::ios::failbit);
  }

  return in;
}

inline std::ostream& operator<<(std::ostream& out, const Double& d) {
  std::array<char, 32> buffer;
  auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), d.value);
  out.write(buffer.data(), ptr - buffer.data());
  return out;
}

struct End_of_file {};

inline const End_of_file eof{};

inline std::istream& operator>>(std::istream& in, const End_of_file& /*eof*/) {
  if (in.eof()) {
    return in;
  }
  in >> std::ws;
  if (in.eof()) {
    return in;
  }
  in.setstate(std::ios::failbit);
  return in;
}

struct Hash_comment {
  bool empty_region{};
  bool full_region{};
};

inline std::istream& operator>>(std::istream& in, Hash_comment& comment) {
  thread_local std::string s;

  if (in.peek() != '#') {
    in.setstate(std::ios::failbit);
    return in;
  }

  while (in.peek() == '#') {
    in.ignore();
  }

  if (in >> s >> eof) {
    // Handle special comments.
    if (s == "empty_region") {
      comment.empty_region = true;
    } else if (s == "full_region") {
      comment.full_region = true;
    }
  }
  in.clear();

  in.ignore(std::numeric_limits<std::streamsize>::max());
  return in;
}

struct String_constant {
  std::string value;
};

inline String_constant operator""_c(const char* str, std::size_t len) {
  return {std::string{str, len}};
}

inline std::istream& operator>>(std::istream& in, const String_constant& str) {
  thread_local std::string s;

  if (!(in >> s) || s != str.value) {
    in.setstate(std::ios::failbit);
  }

  return in;
}

}  // namespace kigumi::io::ascii
