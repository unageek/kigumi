#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/self_intersections.h>
#include <kigumi/Region.h>
#include <kigumi/Region_io.h>

#include <array>
#include <boost/program_options.hpp>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "commands.h"

namespace PMP = CGAL::Polygon_mesh_processing;
using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = typename K::Point_3;
using Region = kigumi::Region<K>;
using kigumi::read_region;

namespace {

struct Options {
  std::string in;
};

}  // namespace

std::string Defects_command::name() const { return "defects"; }

std::string Defects_command::description() const { return "find defects in the given mesh"; }

void Defects_command::operator()(const std::vector<std::string>& args) const {
  namespace po = boost::program_options;

  Options opts;

  po::options_description opts_desc("Options", 80, 50);
  opts_desc.add_options()  //
      ("in", po::value(&opts.in)->required()->value_name("<file>"),
       "the input mesh")  //
      ;

  po::variables_map vm;
  try {
    po::store(po::command_line_parser{args}
                  .options(opts_desc)
                  .positional(po::positional_options_description{}.add("in", 1))
                  .run(),
              vm);
    po::notify(vm);
  } catch (const std::exception&) {
    std::cerr << "usage: kigumi defects [--in] <file>\n"
                 "\n"
              << opts_desc;
    throw;
  }

  Region region;
  if (!read_region(opts.in, region)) {
    throw std::runtime_error("reading failed: " + opts.in);
  }

  const auto& m = region.boundary();

  std::vector<Point> points;
  for (auto vh : m.vertices()) {
    points.push_back(m.point(vh));
  }

  std::vector<std::array<std::size_t, 3>> faces;
  for (auto fh : m.faces()) {
    const auto& f = m.face(fh);
    faces.push_back({f[0].i, f[1].i, f[2].i});
  }

  if (PMP::does_triangle_soup_self_intersect(points, faces)) {
    std::cout << "self-intersecting" << std::endl;
  }
}
