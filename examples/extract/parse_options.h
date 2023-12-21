#pragma once

#include <kigumi/Operator.h>

#include <boost/program_options.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

struct options {
  std::string input_file;
  std::string output_file;
  kigumi::Operator op;
};

inline options parse_options(int argc, const char* argv[]) {
  namespace po = boost::program_options;

  options opts;

  po::options_description opts_desc("Options", 80, 50);
  opts_desc.add_options()                                         //
      ("in", po::value(&opts.input_file)->required(),             //
       "The intermediate result produced by the boole program.")  //
      ("out", po::value(&opts.output_file)->required(),           //
       "The output mesh.")                                        //
      ("op", po::value(&opts.op)->required(),                     //
       "The Boolean operator to apply. Possible values are:\n"    //
       "  uni  Union\n"                                           //
       "  int  Intersection\n"                                    //
       "  dif  Difference\n"                                      //
       "  sym  Symmetric difference");

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, opts_desc), vm);
    po::notify(vm);
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl
              << "Usage: " << argv[0] << " [OPTION]..." << std::endl
              << opts_desc;
    throw;
  }

  return opts;
}

namespace kigumi {

void validate(boost::any& v, const std::vector<std::string>& values, Operator*, int) {
  namespace po = boost::program_options;

  po::validators::check_first_occurrence(v);

  const auto& s = po::validators::get_single_string(values);

  if (s == "dif") {
    v = Operator::Difference;
  } else if (s == "int") {
    v = Operator::Intersection;
  } else if (s == "sym") {
    v = Operator::SymmetricDifference;
  } else if (s == "uni") {
    v = Operator::Union;
  } else {
    throw po::validation_error(po::validation_error::invalid_option_value);
  }
}

}  // namespace kigumi
