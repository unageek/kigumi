#pragma once

#include <CGAL/number_utils.h>
#include <kigumi/Context.h>
#include <kigumi/Mesh_handles.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/io/ascii.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace kigumi::io {

class Write_obj_options {
 public:
  bool negative_indices() const { return negative_indices_; }

  void set_negative_indices(bool negative_indices) { negative_indices_ = negative_indices; }

 private:
  bool negative_indices_{};
};

using Write_obj_context = Context<Write_obj_options>;

template <class K, class FaceData>
bool read_obj(std::istream& is, Triangle_soup<K, FaceData>& soup) {
  using namespace kigumi::io::ascii;
  using Triangle_soup = Triangle_soup<K, FaceData>;

  if (!is) {
    return false;
  }

  Triangle_soup new_soup;

  std::string line;
  std::string s;
  std::vector<Vertex_handle> face;

  while (std::getline(is, line)) {
    std::istringstream iss{line};

    if (iss >> opt_hash_comment_eof) {
      continue;
    }
    iss.clear();

    iss >> s;
    if (s == "v") {
      Double x;
      Double y;
      Double z;
      if (!(iss >> x >> y >> z)) {
        std::cerr << "invalid vertex line: " << line << std::endl;
        return false;
      }
      new_soup.add_vertex({x.value, y.value, z.value});
    } else if (s == "f") {
      face.clear();
      std::ptrdiff_t v{};
      while (iss >> v) {
        if (v > 0) {
          face.push_back(Vertex_handle{static_cast<std::size_t>(v) - 1});
        } else if (v < 0) {
          face.push_back(Vertex_handle{new_soup.num_vertices() + v});
        } else {
          std::cerr << "invalid face line: " << line << std::endl;
          return false;
        }
        // Ignore optional texture and normal indices.
        auto c = iss.peek();
        while (!std::isspace(c) && c != std::char_traits<char>::eof()) {
          iss.ignore();
          c = iss.peek();
        }
      }
      if (face.size() >= 3) {
        for (std::size_t i = 0; i < face.size() - 2; ++i) {
          new_soup.add_face({face.at(0), face.at(i + 1), face.at(i + 2)});
        }
      }
    }
  }

  soup = std::move(new_soup);
  return true;
}

template <class K, class FaceData>
bool read_obj(const std::string& filename, Triangle_soup<K, FaceData>& soup) {
  std::ifstream ifs{filename};
  if (!ifs) {
    std::cerr << "failed to open file: " << filename << std::endl;
    return false;
  }
  return read_obj<K, FaceData>(ifs, soup);
}

template <class K, class FaceData>
bool write_obj(std::ostream& os, const Triangle_soup<K, FaceData>& soup) {
  using namespace kigumi::io::ascii;

  if (!os) {
    return false;
  }

  for (auto vh : soup.vertices()) {
    const auto& p = soup.point(vh);
    p.exact();
    Double x{CGAL::to_double(p.x())};
    Double y{CGAL::to_double(p.y())};
    Double z{CGAL::to_double(p.z())};
    os << "v " << x << ' ' << y << ' ' << z << '\n';
  }

  for (auto fh : soup.faces()) {
    const auto& f = soup.face(fh);
    if (Write_obj_context::current().negative_indices()) {
      auto nv = soup.num_vertices();
      os << "f " << -static_cast<std::ptrdiff_t>(nv - f[0].i) << ' '
         << -static_cast<std::ptrdiff_t>(nv - f[1].i) << ' '
         << -static_cast<std::ptrdiff_t>(nv - f[2].i) << '\n';
    } else {
      os << "f " << f[0].i + 1 << ' ' << f[1].i + 1 << ' ' << f[2].i + 1 << '\n';
    }
  }

  return os.good();
}

template <class K, class FaceData>
bool write_obj(const std::string& filename, const Triangle_soup<K, FaceData>& soup) {
  std::ofstream ofs{filename};
  if (!ofs) {
    std::cerr << "failed to open file: " << filename << std::endl;
    return false;
  }
  return write_obj<K, FaceData>(ofs, soup);
}

}  // namespace kigumi::io
