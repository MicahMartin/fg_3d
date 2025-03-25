#include "CommandScanner.h"
#include "CommandCompiler.h"
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <glaze/glaze.hpp>
#include <glaze/json/read.hpp>

CommandCompiler::CommandCompiler(){}

CommandCompiler::~CommandCompiler(){}

void CommandCompiler::init(const char* path, 
                           bool (*isPressedFn)(void* ctx, uint16_t, bool), 
                           bool (*wasPressedFn)(void* ctx, uint16_t, bool, bool, int), 
                           void* ctx) {

  isPressedFnPtr = isPressedFn;
  wasPressedFnPtr = wasPressedFn;
  apiContext = ctx;
  std::ifstream configFile(path);
  if(!configFile)
    throw std::runtime_error("Failed to open file: " + std::string(path));

  std::string jsonBuff((std::istreambuf_iterator<char>(configFile)), std::istreambuf_iterator<char>());
  auto json = glz::read_json<RootJson>(jsonBuff);
  
  for(auto commandObj : json->commands){
    printf("new command!\n");
    compile(commandObj.command.c_str(), commandObj.clears);
  }

  printf("done compiling commands\n");
}

void CommandCompiler::compile(const char* inputString, bool clears) {
  CommandObj commandObj;
  commandObj.clears = clears;

  std::vector<CommandToken> tokens = commandScanner.scan(inputString);
  currentToken = &tokens[0];

  while(currentToken->type != CTOKEN_END){
    commandObj.command.push_back(compileNode());
    printf("node compiled\n");
  }
  commands.push_back(commandObj);
}

CommandNode CommandCompiler::compileNode(){
  return CommandNode();
}


CommandNode CommandCompiler::compileOneNode(){ return CommandNode(); }

CommandFunction CommandCompiler::binaryCommand(CommandFunction currentFunc, CommandTokenType type){ return CommandFunction();}
