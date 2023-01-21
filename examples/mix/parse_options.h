#pragma once

#include <boost/program_options.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

struct options {
  std::string first;
  std::string second;
  std::string output_file;
};

inline options parse_options(int argc, const char* argv[]) {
  namespace po = boost::program_options;

  options opts;

  po::options_description opts_desc("Options", 80, 50);
  opts_desc.add_options()                                //
      ("first", po::value(&opts.first)->required(),      //
       "The first mesh.")                                //
      ("second", po::value(&opts.second)->required(),    //
       "The second mesh.")                               //
      ("out", po::value(&opts.output_file)->required(),  //
       "The intermediate result to be passed to the extract program.");

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
