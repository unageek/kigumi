#pragma once

#include <gmpxx.h>

#include <iostream>
#include <vector>

namespace kigumi {

template <class T>
void my_write(std::ostream& out, const T& x) {
  out.write(reinterpret_cast<const char*>(&x), sizeof(x));
}

template <class T>
void my_read(std::istream& in, T& x) {
  in.read(reinterpret_cast<char*>(&x), sizeof(x));
}

template <>
inline void my_write(std::ostream& out, const mpq_class& x) {
  auto neg = static_cast<std::int8_t>(x.get_num() < 0);
  out.write(reinterpret_cast<const char*>(&neg), sizeof(neg));

  std::size_t count{};
  auto* raw = mpz_export(nullptr, &count, 1, 1, 0, 0, x.get_num().get_mpz_t());
  out.write(reinterpret_cast<const char*>(&count), sizeof(count));
  out.write(reinterpret_cast<const char*>(raw), static_cast<std::streamsize>(count));
  // NOLINTNEXTLINE(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory)
  std::free(raw);

  raw = mpz_export(nullptr, &count, 1, 1, 0, 0, x.get_den().get_mpz_t());
  out.write(reinterpret_cast<const char*>(&count), sizeof(count));
  out.write(reinterpret_cast<const char*>(raw), static_cast<std::streamsize>(count));
  // NOLINTNEXTLINE(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory)
  std::free(raw);
}

template <>
inline void my_read(std::istream& in, mpq_class& x) {
  mpz_class num;
  mpz_class den;
  std::size_t count{};
  std::vector<std::uint8_t> v;

  std::int8_t neg{};
  in.read(reinterpret_cast<char*>(&neg), sizeof(neg));

  in.read(reinterpret_cast<char*>(&count), sizeof(count));
  v.resize(count);
  in.read(reinterpret_cast<char*>(v.data()), static_cast<std::streamsize>(count));
  mpz_import(num.get_mpz_t(), count, 1, 1, 0, 0, v.data());

  in.read(reinterpret_cast<char*>(&count), sizeof(count));
  v.resize(count);
  in.read(reinterpret_cast<char*>(v.data()), static_cast<std::streamsize>(count));
  mpz_import(den.get_mpz_t(), count, 1, 1, 0, 0, v.data());

  x = mpq_class(num) / mpq_class(den);
  if (static_cast<bool>(neg)) {
    x = -x;
  }
}

}  // namespace kigumi
