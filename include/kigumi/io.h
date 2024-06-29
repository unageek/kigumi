#pragma once

#include <boost/endian/conversion.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace kigumi {

template <class To, class From>
To checked_cast(From value) {
  if (!std::in_range<To>(value)) {
    throw std::runtime_error("checked_cast failed");
  }

  return static_cast<To>(value);
}

template <class T>
struct Write {
  static void write(std::ostream& out, const T& tt) {
    static_assert(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>);

    T x{tt};
    boost::endian::native_to_little_inplace(x);
    out.write(reinterpret_cast<const char*>(&x), sizeof(T));
  }
};

template <class T>
struct Read {
  static void read(std::istream& in, T& tt) {
    static_assert(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>);

    in.read(reinterpret_cast<char*>(&tt), sizeof(T));
    boost::endian::little_to_native_inplace(tt);
  }
};

template <class U, class T, std::enable_if_t<std::is_same_v<U, T>, std::nullptr_t> = nullptr>
void do_write(std::ostream& out, const T& tt) {
  Write<T>::write(out, tt);
}

template <class U, class T, std::enable_if_t<!std::is_same_v<U, T>, std::nullptr_t> = nullptr>
void do_write(std::ostream& out, const T& tt) {
  Write<U>::write(out, checked_cast<U>(tt));
}

template <class U, class T, std::enable_if_t<std::is_same_v<U, T>, std::nullptr_t> = nullptr>
void do_read(std::istream& in, T& tt) {
  Read<T>::read(in, tt);
}

template <class U, class T, std::enable_if_t<!std::is_same_v<U, T>, std::nullptr_t> = nullptr>
void do_read(std::istream& in, T& tt) {
  U x{};
  Read<U>::read(in, x);
  tt = checked_cast<T>(x);
}

template <>
struct Write<bool> {
  static void write(std::ostream& out, const bool& tt) {
    do_write<std::uint8_t>(out, static_cast<std::uint8_t>(tt ? 1 : 0));
  }
};

template <>
struct Read<bool> {
  static void read(std::istream& in, bool& tt) {
    std::uint8_t x{};
    do_read<std::uint8_t>(in, x);
    tt = x != 0;
  }
};

#ifdef CGAL_USE_BOOST_MP

#include <CGAL/boost_mp.h>

template <>
struct Write<boost::multiprecision::cpp_rational> {
  static void write(std::ostream& out, const boost::multiprecision::cpp_rational& tt) {
    do_write<bool>(out, tt < 0);

    std::vector<std::uint8_t> buf;

    export_bits(numerator(tt), std::back_inserter(buf), 8);
    do_write<std::int32_t>(out, buf.size());
    out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(buf.size()));

    buf.clear();

    export_bits(denominator(tt), std::back_inserter(buf), 8);
    do_write<std::int32_t>(out, buf.size());
    out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(buf.size()));
  }
};

template <>
struct Read<boost::multiprecision::cpp_rational> {
  static void read(std::istream& in, boost::multiprecision::cpp_rational& tt) {
    bool neg{};
    do_read<bool>(in, neg);

    boost::multiprecision::cpp_int num;
    boost::multiprecision::cpp_int den;
    std::size_t count{};
    std::vector<std::uint8_t> buf;

    do_read<std::int32_t>(in, count);
    buf.resize(count);
    in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(count));
    import_bits(num, buf.begin(), buf.end());

    do_read<std::int32_t>(in, count);
    buf.resize(count);
    in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(count));
    import_bits(den, buf.begin(), buf.end());

    tt = boost::multiprecision::cpp_rational{num, den};
    if (neg) {
      tt = -tt;
    }
  }
};

#endif

#ifdef CGAL_USE_GMPXX

#include <CGAL/gmpxx.h>

template <>
struct Write<mpq_class> {
  static void write(std::ostream& out, const mpq_class& tt) {
    do_write<bool>(out, tt < 0);

    std::size_t count{};
    std::vector<std::uint8_t> buf;

    buf.resize((mpz_sizeinbase(tt.get_num().get_mpz_t(), 2) + 7) / 8);
    mpz_export(buf.data(), &count, 1, 1, 0, 0, tt.get_num().get_mpz_t());
    do_write<std::int32_t>(out, count);
    out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(count));

    buf.resize((mpz_sizeinbase(tt.get_den().get_mpz_t(), 2) + 7) / 8);
    mpz_export(buf.data(), &count, 1, 1, 0, 0, tt.get_den().get_mpz_t());
    do_write<std::int32_t>(out, count);
    out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(count));
  }
};

template <>
struct Read<mpq_class> {
  static void read(std::istream& in, mpq_class& tt) {
    bool neg{};
    do_read<bool>(in, neg);

    mpz_class num;
    mpz_class den;
    std::size_t count{};
    std::vector<std::uint8_t> buf;

    do_read<std::int32_t>(in, count);
    buf.resize(count);
    in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(count));
    mpz_import(num.get_mpz_t(), count, 1, 1, 0, 0, buf.data());

    do_read<std::int32_t>(in, count);
    buf.resize(count);
    in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(count));
    mpz_import(den.get_mpz_t(), count, 1, 1, 0, 0, buf.data());

    tt = mpq_class{num} / mpq_class{den};
    if (neg) {
      tt = -tt;
    }
  }
};

#endif

template <class T>
void save(const std::string& filename, const T& tt) {
  std::ofstream out(filename, std::ios::binary);
  if (!out) {
    throw std::runtime_error{"Failed to open file: " + filename};
  }

  do_write<T>(out, tt);
}

template <class T>
void load(const std::string& filename, T& tt) {
  std::ifstream in(filename, std::ios::binary);
  if (!in) {
    throw std::runtime_error{"Failed to open file: " + filename};
  }

  do_read<T>(in, tt);
}

}  // namespace kigumi
