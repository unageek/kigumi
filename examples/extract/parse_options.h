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
  bool extract_first{};
  bool extract_second{};
  bool prefer_first{};
};

inline options parse_options(int argc, const char* argv[]) {
  namespace po = boost::program_options;

  options opts;
  std::string op;
  std::string extract;
  std::string prefer;

  po::options_description opts_desc("Options", 80, 50);
  opts_desc.add_options()                                         //
      ("in", po::value(&opts.input_file)->required(),             //
       "The intermediate result produced by the boole program.")  //
      ("out", po::value(&opts.output_file)->required(),           //
       "The output mesh.")                                        //
      ("op", po::value(&op)->required(),                          //
       "The Boolean operator to apply. Possible values are:\n"    //
       "  uni  Union\n"                                           //
       "  int  Intersection\n"                                    //
       "  dif  Difference\n"                                      //
       "  sym  Symmetric difference")                             //
      ("extract", po::value(&extract)->default_value("both"),     //
       "(first|second|both)\n"                                    //
       "Which mesh faces should be extracted?")                   //
      ("prefer", po::value(&prefer)->default_value("first"),      //
       "(first|second)\n"                                         //
       "Which mesh should coplanar faces belong to?");

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

  if (op == "dif") {
    opts.op = kigumi::Operator::Difference;
  } else if (op == "int") {
    opts.op = kigumi::Operator::Intersection;
  } else if (op == "sym") {
    opts.op = kigumi::Operator::SymmetricDifference;
  } else if (op == "uni") {
    opts.op = kigumi::Operator::Union;
  } else {
    throw std::runtime_error("Invalid value to the option --op.");
  }

  if (extract == "first") {
    opts.extract_first = true;
  } else if (extract == "second") {
    opts.extract_second = true;
  } else if (extract == "both") {
    opts.extract_first = true;
    opts.extract_second = true;
  } else {
    throw std::runtime_error("Invalid value to the option --extract.");
  }

  if (prefer == "first") {
    opts.prefer_first = true;
  } else if (prefer == "second") {
    // no-op
  } else {
    throw std::runtime_error("Invalid value to the option --prefer.");
  }

  return opts;
}
