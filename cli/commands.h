#pragma once

#include <string>
#include <vector>

class Command {
 public:
  Command() = default;
  virtual ~Command() = default;
  Command(const Command&) = default;
  Command(Command&&) = default;
  Command& operator=(const Command&) = default;
  Command& operator=(Command&&) = default;

  virtual std::string name() const = 0;

  virtual std::string description() const = 0;

  virtual void operator()(const std::vector<std::string>& args) const = 0;
};

class Boolean_command : public Command {
 public:
  std::string name() const override;

  std::string description() const override;

  void operator()(const std::vector<std::string>& args) const override;
};

class Convert_command : public Command {
 public:
  std::string name() const override;

  std::string description() const override;

  void operator()(const std::vector<std::string>& args) const override;
};

class Defects_command : public Command {
 public:
  std::string name() const override;

  std::string description() const override;

  void operator()(const std::vector<std::string>& args) const override;
};
