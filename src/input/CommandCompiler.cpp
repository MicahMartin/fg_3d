#include <cstdio>
#include <fstream>
#include "CommandCompiler.h"
#include "glaze/json/read.hpp"
#include <glaze/glaze.hpp>

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
CommandCompiler::CommandCompiler(){}

CommandCompiler::~CommandCompiler(){}

void CommandCompiler::init(const char* path, 
            bool (*isPressedFn)(void* ctx, uint16_t, bool),
            bool (*wasPressedFn)(void* ctx, uint16_t, bool, bool, int), 
            void* apiContext) {

  std::ifstream configFile(path);

  if(!configFile){
    printf("what the faaak %s", path);
    throw std::runtime_error("Failed to open file: " + std::string(path));
  }

  std::string json((std::istreambuf_iterator<char>(configFile)), std::istreambuf_iterator<char>());

  CommandJson cmd;
  auto err = glz::read_json(cmd, json);  // Deserialize into struct

  if (err) {
    throw std::runtime_error("Failed to parse JSON: " + std::to_string(err));
  }

  commandStrings.clear();
  commands.clear();

  // for (int i = 0; i < commandStrings.size(); ++i) {
  //  compile(commandStrings[i].command.c_str(), commandStrings[i].clears);
  // }

  printf("done compiling commands\n");
}

void CommandCompiler::compile(const char* inputString, bool clears) {
}

CommandNode CommandCompiler::compileNode(){ }


CommandNode CommandCompiler::compileOneNode(){ }

CommandFunction CommandCompiler::binaryCommand(CommandFunction currentFunc, CommandTokenType type){ }
