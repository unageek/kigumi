#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
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

namespace {

struct Options {
  std::string in;
  std::string out;
};

}  // namespace

std::string Convert_command::name() const { return "convert"; }

std::string Convert_command::description() const { return "convert between mesh formats"; }

void Convert_command::operator()(const std::vector<std::string>& args) const {
  namespace po = boost::program_options;

  Options opts;

  po::options_description opts_desc("Options", 80, 50);
  opts_desc.add_options()  //
      ("in", po::value(&opts.in)->required()->value_name("<file>"),
       "the input mesh")  //
      ("out", po::value(&opts.out)->required()->value_name("<file>"),
       "the output mesh")  //
      ;

  po::variables_map vm;
  try {
    po::store(po::command_line_parser{args}
                  .options(opts_desc)
                  .positional(po::positional_options_description{}.add("in", 1).add("out", 1))
                  .run(),
              vm);
    po::notify(vm);
  } catch (const std::exception&) {
    std::cerr << "usage: kigumi convert [--in] <file> [--out] <file>\n"
                 "\n"
              << opts_desc;
    throw;
  }

  Region region;
  if (!read_region(opts.in, region)) {
    throw std::runtime_error("reading failed: " + opts.in);
  }
  if (!write_region(opts.out, region)) {
    throw std::runtime_error("writing failed: " + opts.out);
  }
}
