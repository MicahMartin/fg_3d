#pragma once

#include <string>
#include <vector>
#include <functional>
#include "CommandScanner.h"

typedef std::function<bool(int, bool)> CommandFunction;

struct CommandNode {
  CommandFunction function;
  int bufferLength;
};

struct CommandObj {
  std::vector<CommandNode> command;
  bool clears;
};

struct CommandStringObj {
  std::string command;
  bool clears;
};

struct CommandJson {
  int num;
  bool clears;
  std::string name;
  std::string command;
};

class CommandCompiler {
public:
  CommandCompiler();
  ~CommandCompiler();

  void init(const char* path, 
            bool (*wasPressedFn)(void* ctx, uint16_t, bool, int, bool), 
            bool (*isPressedFn)(void* ctx, uint16_t, bool),
            void* apiContext);

  void compile(const char* inputString, bool clears);

  CommandNode compileNode();
  CommandNode compileOneNode();
  CommandFunction binaryCommand(CommandFunction currentFunc, CommandTokenType type);

  std::vector<CommandStringObj> commandStrings;
  std::vector<CommandObj> commands;
private:
  void* apiContext; 
  bool (*wasPressedFnPtr)(uint16_t input, bool strict, int index, bool pressed);
  bool (*isPressedFnPtr)(uint16_t input, bool strict);

  CommandScanner commandScanner;
  CommandToken* currentToken;
  
};
