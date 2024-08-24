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
  void operator()(std::ostream& out, const T& t) const {
    static_assert(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>);

    T x{t};
    boost::endian::native_to_little_inplace(x);
    out.write(reinterpret_cast<const char*>(&x), sizeof(T));
  }
};

template <class T>
struct Read {
  void operator()(std::istream& in, T& t) const {
    static_assert(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>);

    in.read(reinterpret_cast<char*>(&t), sizeof(T));
    boost::endian::little_to_native_inplace(t);
  }
};

template <class U, class T, std::enable_if_t<std::is_same_v<U, T>, std::nullptr_t> = nullptr>
void kigumi_write(std::ostream& out, const T& t) {
  Write<T>{}(out, t);
}

template <class U, class T, std::enable_if_t<!std::is_same_v<U, T>, std::nullptr_t> = nullptr>
void kigumi_write(std::ostream& out, const T& t) {
  Write<U>{}(out, checked_cast<U>(t));
}

template <class U, class T, std::enable_if_t<std::is_same_v<U, T>, std::nullptr_t> = nullptr>
void kigumi_read(std::istream& in, T& t) {
  Read<T>{}(in, t);
}

template <class U, class T, std::enable_if_t<!std::is_same_v<U, T>, std::nullptr_t> = nullptr>
void kigumi_read(std::istream& in, T& t) {
  U x{};
  Read<U>{}(in, x);
  t = checked_cast<T>(x);
}

template <>
struct Write<bool> {
  void operator()(std::ostream& out, const bool& t) const {
    kigumi_write<std::uint8_t>(out, static_cast<std::uint8_t>(t ? 1 : 0));
  }
};

template <>
struct Read<bool> {
  void operator()(std::istream& in, bool& t) const {
    std::uint8_t x{};
    kigumi_read<std::uint8_t>(in, x);
    t = x != 0;
  }
};

#ifdef CGAL_USE_BOOST_MP

#include <CGAL/boost_mp.h>

template <>
struct Write<boost::multiprecision::cpp_rational> {
  void operator()(std::ostream& out, const boost::multiprecision::cpp_rational& t) const {
    kigumi_write<bool>(out, t < 0);

    std::vector<std::uint8_t> buf;

    export_bits(numerator(t), std::back_inserter(buf), 8);
    kigumi_write<std::int32_t>(out, buf.size());
    out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(buf.size()));

    buf.clear();

    export_bits(denominator(t), std::back_inserter(buf), 8);
    kigumi_write<std::int32_t>(out, buf.size());
    out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(buf.size()));
  }
};

template <>
struct Read<boost::multiprecision::cpp_rational> {
  void operator()(std::istream& in, boost::multiprecision::cpp_rational& t) const {
    bool neg{};
    kigumi_read<bool>(in, neg);

    boost::multiprecision::cpp_int num;
    boost::multiprecision::cpp_int den;
    std::size_t count{};
    std::vector<std::uint8_t> buf;

    kigumi_read<std::int32_t>(in, count);
    buf.resize(count);
    in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(count));
    import_bits(num, buf.begin(), buf.end());

    kigumi_read<std::int32_t>(in, count);
    buf.resize(count);
    in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(count));
    import_bits(den, buf.begin(), buf.end());

    t = boost::multiprecision::cpp_rational{num, den};
    if (neg) {
      t = -t;
    }
  }
};

#endif

#ifdef CGAL_USE_GMPXX

#include <CGAL/gmpxx.h>

template <>
struct Write<mpq_class> {
  void operator()(std::ostream& out, const mpq_class& t) const {
    kigumi_write<bool>(out, t < 0);

    std::size_t count{};
    std::vector<std::uint8_t> buf;

    buf.resize((mpz_sizeinbase(t.get_num().get_mpz_t(), 2) + 7) / 8);
    mpz_export(buf.data(), &count, 1, 1, 0, 0, t.get_num().get_mpz_t());
    kigumi_write<std::int32_t>(out, count);
    out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(count));

    buf.resize((mpz_sizeinbase(t.get_den().get_mpz_t(), 2) + 7) / 8);
    mpz_export(buf.data(), &count, 1, 1, 0, 0, t.get_den().get_mpz_t());
    kigumi_write<std::int32_t>(out, count);
    out.write(reinterpret_cast<const char*>(buf.data()), static_cast<std::streamsize>(count));
  }
};

template <>
struct Read<mpq_class> {
  void operator()(std::istream& in, mpq_class& t) const {
    bool neg{};
    kigumi_read<bool>(in, neg);

    mpz_class num;
    mpz_class den;
    std::size_t count{};
    std::vector<std::uint8_t> buf;

    kigumi_read<std::int32_t>(in, count);
    buf.resize(count);
    in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(count));
    mpz_import(num.get_mpz_t(), count, 1, 1, 0, 0, buf.data());

    kigumi_read<std::int32_t>(in, count);
    buf.resize(count);
    in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(count));
    mpz_import(den.get_mpz_t(), count, 1, 1, 0, 0, buf.data());

    t = mpq_class{num} / mpq_class{den};
    if (neg) {
      t = -t;
    }
  }
};

#endif

template <class T>
void save(const std::string& filename, const T& t) {
  std::ofstream out(filename, std::ios::binary);
  if (!out) {
    throw std::runtime_error{"failed to open file '" + filename + "'"};
  }

  kigumi_write<T>(out, t);
}

template <class T>
void load(const std::string& filename, T& t) {
  std::ifstream in(filename, std::ios::binary);
  if (!in) {
    throw std::runtime_error{"failed to open file '" + filename + "'"};
  }

  kigumi_read<T>(in, t);
}

}  // namespace kigumi
