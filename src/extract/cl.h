#pragma once

#include <kigumi/Operator.h>

#include <boost/program_options.hpp>
#include <stdexcept>
#include <string>

struct options {
  std::string input_file;
  std::string output_file;
  kigumi::Operator op;
  bool extract_first{};
  bool extract_second{};
  bool prefer_second{};
};

inline options parse_options(int argc, const char* argv[]) {
  namespace po = boost::program_options;

  options opts;
  std::string op;
  std::string extract;

  po::options_description opts_desc("Options", 80, 50);
  opts_desc.add_options()                                                  //
      ("in", po::value(&opts.input_file)->required(),                      //
       "")                                                                 //
      ("out", po::value(&opts.output_file)->required(),                    //
       "")                                                                 //
      ("op", po::value(&op)->required(),                                   //
       "The Boolean operator to apply.")                                   //
      ("extract", po::value(&extract)->default_value("both"),              //
       "(first|second|both) Extract the faces from the first mesh only.")  //
      ("prefer-second", po::bool_switch(&opts.prefer_second),              //
       "Treat coplanar faces as belonging to the second mesh.");

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

  if (op == "uni") {
    opts.op = kigumi::Operator::Union;
  } else if (op == "sym") {
    opts.op = kigumi::Operator::SymmetricDifference;
  } else if (op == "int") {
    opts.op = kigumi::Operator::Intersection;
  } else if (op == "dif") {
    opts.op = kigumi::Operator::Difference;
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

  return opts;
}
