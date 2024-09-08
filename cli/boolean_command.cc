#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <kigumi/Boolean_operator.h>
#include <kigumi/Boolean_region_builder.h>
#include <kigumi/Region.h>
#include <kigumi/Warnings.h>

#include <boost/any.hpp>
#include <boost/program_options.hpp>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "commands.h"
#include "utility.h"

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Region = kigumi::Region<K>;
using kigumi::Boolean_region_builder;

namespace boost {

inline void validate(boost::any& v, const std::vector<std::string>& values,
                     std::optional<std::string>*, int) {
  namespace po = boost::program_options;

  po::validators::check_first_occurrence(v);

  const auto& s = po::validators::get_single_string(values);
  v = std::make_optional(s);
}

}  // namespace boost

namespace {

struct Options {
  std::string first;
  std::string second;
  std::optional<std::string> output_int;
  std::optional<std::string> output_dif;
  std::optional<std::string> output_sym;
  std::optional<std::string> output_uni;
};

}  // namespace

using K = CGAL::Exact_predicates_exact_constructions_kernel;

std::string Boolean_command::name() const { return "boolean"; }

std::string Boolean_command::description() const {
  return "apply boolean operations to two meshes";
}

void Boolean_command::operator()(const std::vector<std::string>& args) const {
  namespace po = boost::program_options;

  Options opts;

  po::options_description opts_desc("Options", 80, 50);
  opts_desc.add_options()  //
      ("first", po::value(&opts.first)->required()->value_name("(<file> | :empty: | :full:)"),
       "the first input mesh")  //
      ("second", po::value(&opts.second)->required()->value_name("(<file> | :empty: | :full:)"),
       "the second input mesh")  //
      ("int", po::value(&opts.output_int)->value_name("<file>"),
       "output the intersection of the two meshes")  //
      ("uni", po::value(&opts.output_uni)->value_name("<file>"),
       "output the union of the two meshes")  //
      ("dif", po::value(&opts.output_dif)->value_name("<file>"),
       "output the difference of the two meshes")  //
      ("sym", po::value(&opts.output_sym)->value_name("<file>"),
       "output the symmetric difference of the two meshes")  //
      ;

  po::variables_map vm;
  try {
    po::store(po::command_line_parser{args}
                  .options(opts_desc)
                  .positional(po::positional_options_description{}.add("first", 1).add("second", 1))
                  .run(),
              vm);
    po::notify(vm);
  } catch (const std::exception&) {
    std::cerr << "usage: kigumi boolean [--first] (<file> | :empty: | :full:)\n"
                 "                      [--second] (<file> | :empty: | :full:)\n"
                 "                      [--int <file>] [--uni <file>] [--dif <file>]\n"
                 "                      [--sym <file>]\n"
                 "\n"
              << opts_desc;
    throw;
  }

  Region first;
  if (!read_region(opts.first, first)) {
    throw std::runtime_error("reading failed: " + opts.first);
  }

  Region second;
  if (!read_region(opts.second, second)) {
    throw std::runtime_error("reading failed: " + opts.second);
  }

  Boolean_region_builder builder{first, second};

  auto warnings = builder.warnings();
  if ((warnings & kigumi::Warnings::FIRST_MESH_PARTIALLY_INTERSECTS_WITH_SECOND_MESH) !=
      kigumi::Warnings::NONE) {
    std::cerr << "warning: the first mesh partially intersects with the second mesh" << std::endl;
  }
  if ((warnings & kigumi::Warnings::SECOND_MESH_PARTIALLY_INTERSECTS_WITH_FIRST_MESH) !=
      kigumi::Warnings::NONE) {
    std::cerr << "warning: the second mesh partially intersects with the first mesh" << std::endl;
  }

  std::vector<std::pair<kigumi::Boolean_operator, std::optional<std::string>>> outputs{
      {kigumi::Boolean_operator::INTERSECTION, opts.output_int},
      {kigumi::Boolean_operator::UNION, opts.output_uni},
      {kigumi::Boolean_operator::DIFFERENCE, opts.output_dif},
      {kigumi::Boolean_operator::SYMMETRIC_DIFFERENCE, opts.output_sym},
  };

  for (const auto& [op, file] : outputs) {
    if (file) {
      auto region = builder(op);
      if (!write_region(*file, region)) {
        throw std::runtime_error("writing failed: " + *file);
      }
    }
  }
}
