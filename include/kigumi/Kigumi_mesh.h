#pragma once

#include <kigumi/Mixed.h>
#include <kigumi/Null_data.h>
#include <kigumi/Operator.h>
#include <kigumi/Side_of_triangle_soup.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/extract.h>
#include <kigumi/io.h>
#include <kigumi/mix.h>

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace kigumi {

enum class Kigumi_mesh_kind {
  Empty,
  Entire,
  Normal,
};

template <>
struct Write<Kigumi_mesh_kind> {
  static void write(std::ostream& out, const Kigumi_mesh_kind& tt) {
    do_write(out, static_cast<std::uint8_t>(tt));
  }
};

template <>
struct Read<Kigumi_mesh_kind> {
  static void read(std::istream& in, Kigumi_mesh_kind& tt) {
    std::uint8_t x{};
    do_read(in, x);
    tt = static_cast<Kigumi_mesh_kind>(x);
  }
};

template <class K, class FaceData>
class Boolean_operation;

template <class K, class FaceData = Null_data>
class Kigumi_mesh {
  using Face_data = FaceData;
  using Point = typename K::Point_3;

 public:
  Kigumi_mesh() = default;

  explicit Kigumi_mesh(Triangle_soup<K, Face_data>&& soup)
      : kind_{soup.num_faces() == 0 ? Kigumi_mesh_kind::Empty : Kigumi_mesh_kind::Normal},
        soup_{std::move(soup)} {}

  static Kigumi_mesh empty() { return {Kigumi_mesh_kind::Empty, {}}; }

  static Kigumi_mesh entire() { return {Kigumi_mesh_kind::Entire, {}}; }

  static Kigumi_mesh import(const std::string& filename) {
    return Kigumi_mesh{Triangle_soup<K, Face_data>{filename}};
  }

  void export_lossy(const std::string& filename) const { soup_.save(filename); }

  bool is_empty() const { return kind_ == Kigumi_mesh_kind::Empty; }

  bool is_entire() const { return kind_ == Kigumi_mesh_kind::Entire; }

  bool is_empty_or_entire() const { return is_empty() || is_entire(); }

  CGAL::Oriented_side side_of_mesh(const Point& p) {
    if (is_empty()) {
      return CGAL::ON_POSITIVE_SIDE;
    }
    if (is_entire()) {
      return CGAL::ON_NEGATIVE_SIDE;
    }
    return Side_of_triangle_soup<K, FaceData>{}(soup(), p);
  }

  // TODO: Prevent inconsistent modification of the polygon soup.
  Triangle_soup<K, Face_data>& soup() { return soup_; }

  const Triangle_soup<K, Face_data>& soup() const { return soup_; }

  Boolean_operation<K, Face_data> boolean(const Kigumi_mesh& other) const {
    const auto& a = *this;
    const auto& b = other;

    if (a.is_empty_or_entire() && b.is_empty_or_entire()) {
      return {a.kind_, b.kind_, {}};
    }

    if (a.is_empty_or_entire() || b.is_empty_or_entire()) {
      std::vector<Point> points;
      points.reserve(a.soup().num_vertices() + b.soup().num_vertices());
      std::transform(a.soup().vertices_begin(), a.soup().vertices_end(), std::back_inserter(points),
                     [&](auto vh) { return a.soup_.point(vh); });
      std::transform(b.soup().vertices_begin(), b.soup().vertices_end(), std::back_inserter(points),
                     [&](auto vh) { return b.soup_.point(vh); });

      std::vector<Face> faces;
      faces.reserve(a.soup().num_faces() + b.soup().num_faces());
      std::transform(a.soup().faces_begin(), a.soup().faces_end(), std::back_inserter(faces),
                     [&](auto fh) { return a.soup_.face(fh); });
      std::transform(b.soup().faces_begin(), b.soup().faces_end(), std::back_inserter(faces),
                     [&](auto fh) { return b.soup_.face(fh); });

      // std::tie is used since structured bindings cannot be captured by lambda expressions.
      Face_tag first_tag{};
      Face_tag second_tag{};
      std::tie(first_tag, second_tag) =
          a.is_empty() || b.is_entire() ? std::make_pair(Face_tag::Intersection, Face_tag::Union)
                                        : std::make_pair(Face_tag::Union, Face_tag::Intersection);

      std::vector<Mixed_face_data<Face_data>> face_data;
      face_data.reserve(faces.size());
      std::transform(a.soup().faces_begin(), a.soup().faces_end(), std::back_inserter(face_data),
                     [&, first_tag](auto fh) -> Mixed_face_data<Face_data> {
                       return {.from_left = true, .tag = first_tag, .data = a.soup().data(fh)};
                     });
      std::transform(b.soup().faces_begin(), b.soup().faces_end(), std::back_inserter(face_data),
                     [&, second_tag](auto fh) -> Mixed_face_data<Face_data> {
                       return {.from_left = false, .tag = second_tag, .data = b.soup().data(fh)};
                     });

      return {a.kind_, b.kind_, {std::move(points), std::move(faces), std::move(face_data)}};
    }

    return {a.kind_, b.kind_, mix(a.soup_, b.soup_)};
  }

 private:
  friend class Boolean_operation<K, Face_data>;
  friend struct Read<Kigumi_mesh>;
  friend struct Write<Kigumi_mesh>;

  Kigumi_mesh(Kigumi_mesh_kind kind, Triangle_soup<K, Face_data>&& soup)
      : kind_{kind}, soup_{std::move(soup)} {}

  Kigumi_mesh_kind kind_{Kigumi_mesh_kind::Normal};
  Triangle_soup<K, Face_data> soup_;
};

template <class K, class FaceData>
struct Write<Kigumi_mesh<K, FaceData>> {
  static void write(std::ostream& out, const Kigumi_mesh<K, FaceData>& tt) {
    do_write(out, tt.kind_);
    do_write(out, tt.soup_);
  }
};

template <class K, class FaceData>
struct Read<Kigumi_mesh<K, FaceData>> {
  static void read(std::istream& in, Kigumi_mesh<K, FaceData>& tt) {
    do_read(in, tt.kind_);
    do_read(in, tt.soup_);
  }
};

template <class K, class FaceData = Null_data>
class Boolean_operation {
 public:
  Boolean_operation()
      : first_kind_{Kigumi_mesh_kind::Empty}, second_kind_{Kigumi_mesh_kind::Empty} {}

  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  Boolean_operation(Kigumi_mesh_kind first_kind, Kigumi_mesh_kind second_kind,
                    Mixed_triangle_soup<K, FaceData>&& m)
      : first_kind_{first_kind}, second_kind_{second_kind}, m_{std::move(m)} {}

  Kigumi_mesh<K, FaceData> apply(Operator op, bool prefer_first = true) const {
    auto soup = extract(m_, op, prefer_first);
    if (soup.num_faces() != 0) {
      return {Kigumi_mesh_kind::Normal, std::move(soup)};
    }

    if (first_kind_ == Kigumi_mesh_kind::Empty || second_kind_ == Kigumi_mesh_kind::Empty) {
      auto a = first_kind_ != Kigumi_mesh_kind::Empty;
      auto b = second_kind_ != Kigumi_mesh_kind::Empty;
      return {apply(op, a, b) ? Kigumi_mesh_kind::Entire : Kigumi_mesh_kind::Empty,
              std::move(soup)};
    }

    if (first_kind_ == Kigumi_mesh_kind::Entire || second_kind_ == Kigumi_mesh_kind::Entire) {
      auto a = first_kind_ == Kigumi_mesh_kind::Entire;
      auto b = second_kind_ == Kigumi_mesh_kind::Entire;
      return {apply(op, a, b) ? Kigumi_mesh_kind::Entire : Kigumi_mesh_kind::Empty,
              std::move(soup)};
    }

    switch (op) {
      case Operator::V:
      case Operator::A:
      case Operator::B:
      case Operator::C:
      case Operator::D:
        return {Kigumi_mesh_kind::Entire, std::move(soup)};

      case Operator::K:
      case Operator::L:
      case Operator::M:
      case Operator::X:
      case Operator::O:
        return {Kigumi_mesh_kind::Empty, std::move(soup)};

      default:
        break;
    }

    if (op == Operator::E || op == Operator::J) {
      if (std::all_of(m_.faces_begin(), m_.faces_end(),
                      [&](auto fh) { return m_.data(fh).tag == Face_tag::Coplanar; })) {
        return {apply(op, false, false) ? Kigumi_mesh_kind::Entire : Kigumi_mesh_kind::Empty,
                std::move(soup)};
      }

      if (std::all_of(m_.faces_begin(), m_.faces_end(),
                      [&](auto fh) { return m_.data(fh).tag == Face_tag::Opposite; })) {
        return {apply(op, false, true) ? Kigumi_mesh_kind::Entire : Kigumi_mesh_kind::Empty,
                std::move(soup)};
      }
    }

    throw std::runtime_error("input meshes are inconsistently oriented");
  }

 private:
  friend struct Read<Boolean_operation>;
  friend struct Write<Boolean_operation>;

  static bool apply(Operator op, bool a, bool b) {
    switch (op) {
      case Operator::V:  // The universe
        return true;
      case Operator::A:  // A ∪ B
        return a || b;
      case Operator::B:  // (B ⧵ A)ᶜ
        return a || !b;
      case Operator::C:  // (A ⧵ B)ᶜ
        return !a || b;
      case Operator::D:  // (A ∩ B)ᶜ
        return !a || !b;
      case Operator::E:  // (A △ B)ᶜ
        return a == b;
      case Operator::F:  // Aᶜ
        return !a;
      case Operator::G:  // Bᶜ
        return !b;
      case Operator::H:  // B
        return b;
      case Operator::I:  // A
        return a;
      case Operator::J:  // A △ B
        return a != b;
      case Operator::K:  // A ∩ B
        return a && b;
      case Operator::L:  // A ⧵ B
        return a && !b;
      case Operator::M:  // B ⧵ A
        return !a && b;
      case Operator::X:  // (A ∪ B)ᶜ
        return !a && !b;
      case Operator::O:  // ∅
        return false;
      default:
        throw std::runtime_error("invalid operator");
    }
  }

  Kigumi_mesh_kind first_kind_;
  Kigumi_mesh_kind second_kind_;
  Mixed_triangle_soup<K, FaceData> m_;
};

template <class K, class FaceData>
struct Write<Boolean_operation<K, FaceData>> {
  static void write(std::ostream& out, const Boolean_operation<K, FaceData>& tt) {
    do_write(out, tt.first_kind_);
    do_write(out, tt.second_kind_);
    do_write(out, tt.m_);
  }
};

template <class K, class FaceData>
struct Read<Boolean_operation<K, FaceData>> {
  static void read(std::istream& in, Boolean_operation<K, FaceData>& tt) {
    do_read(in, tt.first_kind_);
    do_read(in, tt.second_kind_);
    do_read(in, tt.m_);
  }
};

}  // namespace kigumi
