#pragma once

#include <CGAL/number_utils.h>
#include <kigumi/Mesh_handles.h>
#include <kigumi/Region.h>
#include <kigumi/Triangle_soup.h>
#include <kigumi/io/ascii.h>
#include <kigumi/io/validate_region.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace kigumi::io {

struct Off_signature {
  bool binary{};
};

inline std::istream& operator>>(std::istream& in, Off_signature& sig) {
  using namespace kigumi::io::ascii;

  std::string s;
  if (!(in >> s)) {
    return in;
  }

  std::string_view sv{s};
  if (sv.starts_with("ST")) {
    sv = sv.substr(2);
  }
  if (sv.starts_with('C')) {
    sv = sv.substr(1);
  }
  if (sv.starts_with('N')) {
    sv = sv.substr(1);
  }
  if (sv != "OFF") {
    in.setstate(std::ios::failbit);
    return in;
  }

  if (in >> eof) {
    return in;
  }
  in.clear();

  if (in >> "BINARY"_c) {
    sig.binary = true;
  }

  return in >> eof;
}

enum class Off_reading_state {
  READING_SIGNATURE,
  READING_NUMBERS,
  READING_VERTICES,
  READING_FACES,
  END,
};

template <class K, class FaceData>
bool read_off(std::istream& is, Region<K, FaceData>& region) {
  using namespace kigumi::io::ascii;
  using Region = Region<K, FaceData>;
  using Triangle_soup = Triangle_soup<K, FaceData>;

  if (!is) {
    return false;
  }

  Triangle_soup soup;
  auto empty = false;
  auto full = false;

  std::size_t num_vertices{};
  std::size_t num_faces{};
  std::string line;
  std::string s;
  std::vector<Vertex_handle> face;

  Off_reading_state state = Off_reading_state::READING_SIGNATURE;

  while (std::getline(is, line)) {
    std::istringstream iss{line};

    if (iss >> eof) {
      continue;
    }
    iss.clear();

    if (iss.peek() == '#') {
      Hash_comment comment;
      iss >> comment;
      empty = empty || comment.empty_region;
      full = full || comment.full_region;
      continue;
    }

    switch (state) {
      case Off_reading_state::READING_SIGNATURE: {
        Off_signature sig;
        if (!(iss >> sig)) {
          std::cerr << "unexpected line: " << line << std::endl;
          return false;
        }
        if (sig.binary) {
          std::cerr << "binary OFF is not supported" << std::endl;
          return false;
        }
        state = Off_reading_state::READING_NUMBERS;
        break;
      }
      case Off_reading_state::READING_NUMBERS: {
        if (!(iss >> num_vertices >> num_faces)) {
          std::cerr << "unexpected line: " << line << std::endl;
          return false;
        }
        state = Off_reading_state::READING_VERTICES;
        if (num_vertices == 0) {
          state = Off_reading_state::READING_FACES;
          if (num_faces == 0) {
            state = Off_reading_state::END;
          }
        }
        break;
      }
      case Off_reading_state::READING_VERTICES: {
        Double x;
        Double y;
        Double z;
        if (!(iss >> x >> y >> z)) {
          std::cerr << "invalid vertex line: " << line << std::endl;
          return false;
        }
        soup.add_vertex({x.value, y.value, z.value});
        --num_vertices;
        if (num_vertices == 0) {
          state = Off_reading_state::READING_FACES;
          if (num_faces == 0) {
            state = Off_reading_state::END;
          }
        }
        break;
      }
      case Off_reading_state::READING_FACES: {
        face.clear();
        std::size_t count{};
        iss >> count;
        for (std::size_t i = 0; i < count; ++i) {
          std::size_t v{};
          iss >> v;
          face.push_back(Vertex_handle{v});
        }
        if (!iss) {
          std::cout << "invalid face line: " << line << std::endl;
          return false;
        }
        if (face.size() >= 3) {
          for (std::size_t i = 0; i < face.size() - 2; ++i) {
            soup.add_face({face.at(0), face.at(i + 1), face.at(i + 2)});
          }
        }
        --num_faces;
        if (num_faces == 0) {
          state = Off_reading_state::END;
        }
        break;
      }
      case Off_reading_state::END:
        std::cout << "unexpected line: " << line << std::endl;
        return false;
    }
  }

  if (state != Off_reading_state::END) {
    std::cerr << "unexpected end of file" << std::endl;
    return false;
  }

  if (!validate_region(soup, empty, full)) {
    return false;
  }

  if (empty) {
    region = Region::empty();
    return true;
  }

  if (full) {
    region = Region::full();
    return true;
  }

  region = Region{std::move(soup)};
  return true;
}

template <class K, class FaceData>
bool read_off(const std::string& filename, Region<K, FaceData>& region) {
  std::ifstream ifs{filename};
  if (!ifs) {
    std::cerr << "failed to open file: " << filename << std::endl;
    return false;
  }
  return read_off<K, FaceData>(ifs, region);
}

template <class K, class FaceData>
bool write_off(std::ostream& os, const Region<K, FaceData>& region) {
  using namespace kigumi::io::ascii;

  if (!os) {
    return false;
  }

  const auto& soup = region.boundary();

  os << "OFF\n"  //
     << soup.num_vertices() << ' ' << soup.num_faces() << " 0\n";

  if (region.is_empty()) {
    os << "# empty_region\n";
  }

  if (region.is_full()) {
    os << "# full_region\n";
  }

  for (auto vh : soup.vertices()) {
    const auto& p = soup.point(vh);
    p.exact();
    Double x{CGAL::to_double(p.x())};
    Double y{CGAL::to_double(p.y())};
    Double z{CGAL::to_double(p.z())};
    os << x << ' ' << y << ' ' << z << '\n';
  }

  for (auto fh : soup.faces()) {
    const auto& f = soup.face(fh);
    os << "3 " << f[0].i << ' ' << f[1].i << ' ' << f[2].i << '\n';
  }

  return os.good();
}

template <class K, class FaceData>
bool write_off(const std::string& filename, const Region<K, FaceData>& region) {
  std::ofstream ofs{filename};
  if (!ofs) {
    std::cerr << "failed to open file: " << filename << std::endl;
    return false;
  }
  return write_off<K, FaceData>(ofs, region);
}

}  // namespace kigumi::io
