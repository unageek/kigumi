#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Region.h>
#include <kigumi/Region_io.h>

#include <boost/program_options.hpp>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "commands.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Face_data = int;
using Region = kigumi::Region<K, Face_data>;
using kigumi::read_region;
using kigumi::write_region;

namespace {

struct Options {
  std::string from;
  std::string to;
};

}  // namespace

std::string Convert_command::name() const { return "convert"; }

std::string Convert_command::description() const { return "convert between mesh formats"; }

void Convert_command::operator()(const std::vector<std::string>& args) const {
  namespace po = boost::program_options;

  Options opts;

  po::options_description opts_desc("Options", 80, 50);
  opts_desc.add_options()  //
      ("from", po::value(&opts.from)->required()->value_name("<file>"),
       "the input mesh")  //
      ("to", po::value(&opts.to)->required()->value_name("<file>"),
       "the output mesh")  //
      ;

  po::variables_map vm;
  try {
    po::store(po::command_line_parser{args}
                  .options(opts_desc)
                  .positional(po::positional_options_description{}.add("from", 1).add("to", 1))
                  .run(),
              vm);
    po::notify(vm);
  } catch (const std::exception&) {
    std::cerr << "usage: kigumi convert [--from] <file> [--to] <file>\n"
                 "\n"
              << opts_desc;
    throw;
  }

  Region region;
  if (!read_region(opts.from, region)) {
    throw std::runtime_error("reading failed: " + opts.from);
  }
  if (!write_region(opts.to, region)) {
    throw std::runtime_error("writing failed: " + opts.to);
  }
}
