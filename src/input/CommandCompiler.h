#pragma once

#include <string>
#include <vector>
#include <functional>
#include "CommandScanner.h"

// each 'commandString' is a descriptor for a stack of boolean function calls
// P | ~P = ((wasPressed(LP)) || (wasReleased(LP)))
// @F & !D = ((wasPressed(F, strict = false)) && !(wasPressed(D)))
// MP & *D = ((wasPressed(MP)) && (isPressed(F)))
// DF = (wasPressed(DF))
// ~D = (wasReleased(D))

//  Forward, neutral, forward
//  "F, N, F",
//  back , neutral, back 
//  "B, N, B",
//  any down release, neutral, any down press, LP
//  "@~D, N, @D, LP",
//  any forward that doesnt include down, neutral, forward (lienent dash)
//  "@F & !D, N, F"
//  any back that doesnt include down, neutral, forward (lienent backdash)
//  "@B & !D, N, B",
//  release of down, downforward, anyforward that doesnt include down, lk or release of lk (236K)
//  "~D, DF, @F & !D, LK | ~LK",
//  release of down, downback, any back that doesnt include down, lp or release of lp (214P)
//  "~D, DB, @B & !D, LP | ~LP",
//  MP + forward IS pressed
//  "MP & *F",
//  MP + back IS pressed 
//  "MP & *B",
//
// input = N, F, B, U, D, UF, UB, DF, DB, LP, LK, MP, MK
// funcMods = ~, *, @
// unary = !
// binary = &, |
// TODO: unary and binary
//  "MP & *F",
//  "MP & *B",
//  "@F & !D, N, F"
//  "@B & !D, N, B",
//  "~D, DF, @F & !D, LK | ~LK",
//  "~D, DB, @B & !D, LP | ~LP",
//  TODO: load from file
//   "~D, 20DF, 20F, 8LP | 8~LP", // 214P

typedef std::function<bool(uint16_t, bool, int, bool)> CommandFunction;

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

  void init(const char* path, 
            bool (*isPressedFn)(void* ctx, uint16_t, bool),
            bool (*wasPressedFn)(void* ctx, uint16_t, bool, bool, int), 
            void* ctx);
private:
  void compile(const char* inputString, bool clears);
  CommandNode compileNode();
  CommandNode compileOneNode();
  CommandFunction binaryCommand(CommandFunction currentFunc, CommandTokenType type);

  std::vector<CommandStringObj> commandStrings;
  std::vector<CommandObj> commands;

  void* apiContext; 
  bool (*wasPressedFnPtr)(void* ctx, uint16_t input, bool strict, bool pressed, int index);
  bool (*isPressedFnPtr)(void* ctx, uint16_t input, bool strict);

  CommandScanner commandScanner;
  CommandToken* currentToken;
};
