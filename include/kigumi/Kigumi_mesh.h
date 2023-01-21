#pragma once

#include <kigumi/Mixed_mesh.h>
#include <kigumi/Operator.h>
#include <kigumi/Polygon_soup.h>
#include <kigumi/extract.h>
#include <kigumi/io.h>
#include <kigumi/mix.h>

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>
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

template <class K, class FaceData = std::nullptr_t>
class Kigumi_mesh {
  using Face_data = FaceData;
  using Point = typename K::Point_3;

 public:
  Kigumi_mesh() = default;

  Kigumi_mesh(Polygon_soup<K, Face_data>&& soup)
      : kind_{soup.num_faces() == 0 ? Kigumi_mesh_kind::Empty : Kigumi_mesh_kind::Normal},
        soup_{std::move(soup)} {}

  static Kigumi_mesh empty() { return {Kigumi_mesh_kind::Empty, {}}; }

  static Kigumi_mesh entire() { return {Kigumi_mesh_kind::Entire, {}}; }

  static Kigumi_mesh import(const std::string& filename) {
    return {Polygon_soup<K, Face_data>{filename}};
  }

  void save_lossy(const std::string& filename) { soup_.save(filename); }

  bool is_empty() const { return kind_ == Kigumi_mesh_kind::Empty; }

  bool is_entire() const { return kind_ == Kigumi_mesh_kind::Entire; }

  bool is_empty_or_entire() const { return is_empty() || is_entire(); }

  const Polygon_soup<K, Face_data>& soup() const { return soup_; }

  static Boolean_operation<K, Face_data> boolean(const Kigumi_mesh& a, const Kigumi_mesh& b) {
    if (a.is_empty_or_entire() && b.is_empty_or_entire()) {
      return {a.kind_, b.kind_, {}};
    }

    if (a.is_empty_or_entire() || b.is_empty_or_entire()) {
      std::vector<Point> points;
      points.reserve(a.soup_.num_vertices() + b.soup_.num_vertices());
      std::transform(a.soup_.vertices_begin(), a.soup_.vertices_end(), std::back_inserter(points),
                     [&](auto vh) { return a.soup_.point(vh); });
      std::transform(b.soup_.vertices_begin(), b.soup_.vertices_end(), std::back_inserter(points),
                     [&](auto vh) { return b.soup_.point(vh); });

      std::vector<Face> faces;
      faces.reserve(a.soup_.num_faces() + b.soup_.num_faces());
      std::transform(a.soup_.faces_begin(), a.soup_.faces_end(), std::back_inserter(faces),
                     [&](auto fh) { return a.soup_.face(fh); });
      std::transform(b.soup_.faces_begin(), b.soup_.faces_end(), std::back_inserter(faces),
                     [&](auto fh) { return b.soup_.face(fh); });

      std::vector<Mixed_face_data<Face_data>> face_data;
      face_data.reserve(faces.size());
      if (a.is_empty() || b.is_entire()) {
        face_data.insert(
            face_data.end(), a.soup_.num_faces(),
            Mixed_face_data<Face_data>{.from_left = true, .tag = Face_tag::Intersection});
        face_data.insert(face_data.end(), b.soup_.num_faces(),
                         Mixed_face_data<Face_data>{.from_left = false, .tag = Face_tag::Union});
      } else {
        face_data.insert(face_data.end(), a.soup_.num_faces(),
                         Mixed_face_data<Face_data>{.from_left = true, .tag = Face_tag::Union});
        face_data.insert(
            face_data.end(), b.soup_.num_faces(),
            Mixed_face_data<Face_data>{.from_left = false, .tag = Face_tag::Intersection});
      }

      return {a.kind_, b.kind_, {std::move(points), std::move(faces), std::move(face_data)}};
    }

    return {a.kind_, b.kind_, mix(a.soup_, b.soup_)};
  }

 private:
  friend class Boolean_operation<K, Face_data>;
  friend struct Read<Kigumi_mesh>;
  friend struct Write<Kigumi_mesh>;

  Kigumi_mesh(Kigumi_mesh_kind kind, Polygon_soup<K, Face_data>&& soup)
      : kind_{kind}, soup_{std::move(soup)} {}

  Kigumi_mesh_kind kind_{Kigumi_mesh_kind::Normal};
  Polygon_soup<K, Face_data> soup_;
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

template <class K, class FaceData>
class Boolean_operation {
 public:
  Boolean_operation()
      : first_kind_(Kigumi_mesh_kind::Empty), second_kind_(Kigumi_mesh_kind::Empty), m_() {}

  Boolean_operation(Kigumi_mesh_kind first_kind, Kigumi_mesh_kind second_kind,
                    Mixed_polygon_soup<K, FaceData>&& m)
      : first_kind_(first_kind), second_kind_(second_kind), m_(std::move(m)) {}

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

    throw std::runtime_error("Input meshes are inconsistently oriented");
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
        return !(b && !a);
      case Operator::C:  // (A ⧵ B)ᶜ
        return !(a && !b);
      case Operator::D:  // (A ∩ B)ᶜ
        return !(a && b);
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
        return b && !a;
      case Operator::X:  // (A ∪ B)ᶜ
        return !(a || b);
      case Operator::O:  // ∅
        return false;
      default:
        throw std::runtime_error("Unknown operator");
    }
  }

  Kigumi_mesh_kind first_kind_;
  Kigumi_mesh_kind second_kind_;
  Mixed_polygon_soup<K, FaceData> m_;
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
