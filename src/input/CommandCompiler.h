#pragma once
#include <string>
#include <vector>
#include "CommandScanner.h"
#include "CommandVm.h"

struct CommandStringObj {
  std::string command;
  bool clears;
};

struct CommandJson {
  std::string command;
  std::string name;
  bool clears;
  int num;
};

struct RootJson {
  std::vector<CommandJson> commands;
};

class CommandCompiler {
public:
  CommandCompiler();
  ~CommandCompiler();

  void init(const char* path);
private:
  void compile(const char* inputString, bool clears);
  CommandCode compileNode();

  std::vector<CommandStringObj> commandStrings;
  std::vector<CommandCode> commands;

  CommandScanner commandScanner;
  CommandToken* currentToken;
};
