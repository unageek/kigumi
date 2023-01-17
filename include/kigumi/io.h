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
  auto neg = static_cast<std::uint8_t>(x.get_num() < 0);
  out.write(reinterpret_cast<const char*>(&neg), sizeof(neg));

  std::size_t count{};
  std::vector<std::uint8_t> buf;

  buf.resize((mpz_sizeinbase(x.get_num().get_mpz_t(), 2) + 7) / 8);
  mpz_export(buf.data(), &count, 1, 1, 0, 0, x.get_num().get_mpz_t());
  out.write(reinterpret_cast<const char*>(&count), sizeof(count));
  out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(count));

  buf.resize((mpz_sizeinbase(x.get_den().get_mpz_t(), 2) + 7) / 8);
  mpz_export(buf.data(), &count, 1, 1, 0, 0, x.get_den().get_mpz_t());
  out.write(reinterpret_cast<const char*>(&count), sizeof(count));
  out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(count));
}

template <>
inline void my_read(std::istream& in, mpq_class& x) {
  std::uint8_t neg{};
  in.read(reinterpret_cast<char*>(&neg), sizeof(neg));

  mpz_class num;
  mpz_class den;
  std::size_t count{};
  std::vector<std::uint8_t> buf;

  in.read(reinterpret_cast<char*>(&count), sizeof(count));
  buf.resize(count);
  in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(count));
  mpz_import(num.get_mpz_t(), count, 1, 1, 0, 0, buf.data());

  in.read(reinterpret_cast<char*>(&count), sizeof(count));
  buf.resize(count);
  in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(count));
  mpz_import(den.get_mpz_t(), count, 1, 1, 0, 0, buf.data());

  x = mpq_class(num) / mpq_class(den);
  if (static_cast<bool>(neg)) {
    x = -x;
  }
}

}  // namespace kigumi
