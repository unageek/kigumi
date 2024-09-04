#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "commands.h"

int main(int argc, const char* argv[]) {
  try {
    std::vector<std::string> args(argv + 1, argv + argc);

    std::vector<std::unique_ptr<Command>> commands;
    commands.emplace_back(std::make_unique<Boolean_command>());
    commands.emplace_back(std::make_unique<Convert_command>());

    if (args.empty()) {
      std::cerr << "usage: kigumi <command> [<args>]\n"
                   "\n"
                   "Commands:\n";
      for (const auto& c : commands) {
        std::cerr << "  " << std::setw(12) << std::left << c->name() << "  " << c->description()
                  << '\n';
      }
      return 1;
    }

    auto command = args.front();
    args.erase(args.begin());

    for (const auto& c : commands) {
      if (c->name() == command) {
        (*c)(args);
        return 0;
      }
    }

    std::cerr << "error: unknown command: " << command << '\n';
    return 1;
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << '\n';
    return 1;
  } catch (...) {
    std::cerr << "unknown error\n";
    return 1;
  }
}
