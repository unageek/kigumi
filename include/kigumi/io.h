#pragma once

#include <CGAL/gmpxx.h>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace kigumi {

template <class T>
struct MyWrite {
  static void my_write(std::ostream& out, const T& tt) {
    out.write(reinterpret_cast<const char*>(&tt), sizeof(tt));
  }
};

template <class T>
struct MyRead {
  static void my_read(std::istream& in, T& tt) {
    in.read(reinterpret_cast<char*>(&tt), sizeof(tt));
  }
};

template <class T>
void do_my_write(std::ostream& out, const T& tt) {
  MyWrite<T>::my_write(out, tt);
}

template <class T>
void do_my_read(std::istream& in, T& tt) {
  MyRead<T>::my_read(in, tt);
}

template <>
struct MyWrite<bool> {
  static void my_write(std::ostream& out, const bool& tt) {
    do_my_write(out, static_cast<std::uint8_t>(tt));
  }
};

template <>
struct MyRead<bool> {
  static void my_read(std::istream& in, bool& tt) {
    std::uint8_t x{};
    do_my_read(in, x);
    tt = static_cast<bool>(x);
  }
};

template <>
struct MyWrite<std::nullptr_t> {
  static void my_write(std::ostream& /*out*/, const std::nullptr_t& /*tt*/) {
    // no-op
  }
};

template <>
struct MyRead<std::nullptr_t> {
  static void my_read(std::istream& /*in*/, std::nullptr_t& /*tt*/) {
    // no-op
  }
};

template <>
struct MyWrite<mpq_class> {
  static void my_write(std::ostream& out, const mpq_class& tt) {
    auto neg = static_cast<std::uint8_t>(tt.get_num() < 0);
    do_my_write(out, neg);

    std::size_t count{};
    std::vector<std::uint8_t> buf;

    buf.resize((mpz_sizeinbase(tt.get_num().get_mpz_t(), 2) + 7) / 8);
    mpz_export(buf.data(), &count, 1, 1, 0, 0, tt.get_num().get_mpz_t());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(count));

    buf.resize((mpz_sizeinbase(tt.get_den().get_mpz_t(), 2) + 7) / 8);
    mpz_export(buf.data(), &count, 1, 1, 0, 0, tt.get_den().get_mpz_t());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(count));
  }
};

template <>
struct MyRead<mpq_class> {
  static void my_read(std::istream& in, mpq_class& tt) {
    std::uint8_t neg{};
    do_my_read(in, neg);

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

    tt = mpq_class(num) / mpq_class(den);
    if (static_cast<bool>(neg)) {
      tt = -tt;
    }
  }
};

template <class T>
void save(const std::string& filename, const T& tt) {
  std::ofstream out(filename, std::ios::binary);
  if (!out) {
    throw std::runtime_error{"Failed to open file: " + filename};
  }

  do_my_write(out, tt);
}

template <class T>
void load(const std::string& filename, T& tt) {
  std::ifstream in(filename, std::ios::binary);
  if (!in) {
    throw std::runtime_error{"Failed to open file: " + filename};
  }

  do_my_read(in, tt);
}

}  // namespace kigumi
