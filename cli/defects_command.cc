#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Find_defects.h>
#include <kigumi/Region.h>

#include <boost/program_options.hpp>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "commands.h"
#include "utility.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Region = kigumi::Region<K>;
using kigumi::Find_defects;

namespace {

struct Options {
  std::string in;
};

}  // namespace

std::string Defects_command::name() const { return "defects"; }

std::string Defects_command::description() const { return "find defects in a mesh"; }

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

  Find_defects defects{region.boundary()};

  const auto& isolated_vertices = defects.isolated_vertices();
  if (!isolated_vertices.empty()) {
    std::cout << isolated_vertices.size() << " isolated vertices" << std::endl;
  }

  const auto& non_manifold_vertices = defects.non_manifold_vertices();
  if (!non_manifold_vertices.empty()) {
    std::cout << non_manifold_vertices.size() << " non-manifold vertices" << std::endl;
  }

  const auto& boundary_edges = defects.boundary_edges();
  if (!boundary_edges.empty()) {
    std::cout << boundary_edges.size() << " boundary edges" << std::endl;
  }

  const auto& inconsistent_edges = defects.inconsistent_edges();
  if (!inconsistent_edges.empty()) {
    std::cout << inconsistent_edges.size() << " inconsistent edges" << std::endl;
  }

  const auto& non_manifold_edges = defects.non_manifold_edges();
  if (!non_manifold_edges.empty()) {
    std::cout << non_manifold_edges.size() << " non-manifold edges" << std::endl;
  }

  const auto& degenerate_faces = defects.degenerate_faces();
  if (!degenerate_faces.empty()) {
    std::cout << degenerate_faces.size() << " degenerate faces" << std::endl;
  }

  const auto& overlapping_faces = defects.overlapping_faces();
  if (!overlapping_faces.empty()) {
    std::cout << overlapping_faces.size() << " overlapping faces" << std::endl;
  }
}
