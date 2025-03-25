#include <cstdio>
#include <fstream>
#include "CommandCompiler.h"
#include "CommandScanner.h"
#include "glaze/json/read.hpp"
#include <glaze/glaze.hpp>

CommandCompiler::CommandCompiler(){}

CommandCompiler::~CommandCompiler(){}

void CommandCompiler::init(const char* path, 
                           bool (*isPressedFn)(void* ctx, uint16_t, bool), 
                           bool (*wasPressedFn)(void* ctx, uint16_t, bool, bool, int), 
                           void* apiContext) {
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
  for(CommandToken tokenToPrint : tokens) {
    printf("t:%s\n", commandScanner.tokenToString[tokenToPrint.type]);
  }

  currentToken = &tokens[0];
  // while(currentToken->type != CTOKEN_END){
  //   commandObj.command.push_back(compileNode());
  // }

  // commands.push_back(commandObj);
}

CommandNode CommandCompiler::compileNode(){
  // function pointer is &VirtualController::wasPressed by default
  // bool strictness is true by default
  using namespace std::placeholders;
  // std::function<bool(Input, bool, int, bool)> funcPointer = std::bind(&VirtualController::wasPressedWrapper, controllerPointer, _1, _2, _3, _4);
  CommandNode finalNode;
  CommandFunction& finalFunc = finalNode.function;
  bool strictness = true;
  finalNode.bufferLength = 8;

  while(currentToken->type != CTOKEN_DELIM && currentToken->type != CTOKEN_END) {
    switch (currentToken->type) {
      case CTOKEN_RELEASED: {
        // funcPointer = std::bind(&VirtualController::wasReleasedWrapper, controllerPointer, _1, _2, _3, _4);
        currentToken++;
      }
      break;
      case CTOKEN_HELD: {
        // funcPointer = std::bind(&VirtualController::isPressedWrapper, controllerPointer, _1, _2, _3, _4);
        currentToken++;
      }
      break;
      case CTOKEN_ANY: {
        strictness = false;
        currentToken++;
      }
      break;
      case CTOKEN_NEUTRAL: {
       // finalFunc = std::bind(funcPointer, NOINPUT, strictness, _1, _2);
       currentToken++;
      }
      break;
      case CTOKEN_FORWARD: {
       // finalFunc = std::bind(funcPointer, RIGHT, strictness, _1, _2);
       currentToken++;
      }
      break;
      case CTOKEN_BACK: {
        // finalFunc = std::bind(funcPointer, LEFT, strictness, _1, _2);
        currentToken++;
      }
      break;
      case CTOKEN_UP: {
        // finalFunc = std::bind(funcPointer, UP, strictness, _1, _2);
        printf("building up\n");
        currentToken++;
      }
      break;
      case CTOKEN_DOWN: {
        // finalFunc = std::bind(funcPointer, DOWN, strictness, _1, _2);
        printf("building down\n");
        currentToken++;
      }
      break;
      case CTOKEN_UPFORWARD: {
        // finalFunc = std::bind(funcPointer, UPRIGHT, strictness, _1, _2);
        printf("building upforward\n");
        currentToken++;
      }
      break;
      case CTOKEN_UPBACK: {
        // finalFunc = std::bind(funcPointer, UPLEFT, strictness, _1, _2);
        printf("building upback\n");
        currentToken++;
      }
      break;
      case CTOKEN_DOWNFORWARD: {
        // finalFunc = std::bind(funcPointer, DOWNRIGHT, strictness, _1, _2);
        printf("building upforward\n");
        currentToken++;
      }
      break;
      case CTOKEN_DOWNBACK: {
        // finalFunc = std::bind(funcPointer, DOWNLEFT, strictness, _1, _2);
        printf("building downback\n");
        currentToken++;
      }
      break;
      case CTOKEN_LP: {
        //finalFunc = std::bind(funcPointer, LP, strictness, _1, _2);
        printf("building lightpunch\n");
        currentToken++;
      }
      break;
      case CTOKEN_LK: {
        // finalFunc = std::bind(funcPointer, LK, strictness, _1, _2);
        printf("building lightk\n");
        currentToken++;
      }
      break;
      case CTOKEN_MP: {
        // finalFunc = std::bind(funcPointer, MP, strictness, _1, _2);
        printf("building mediumP\n");
        currentToken++;
      }
      break;
      case CTOKEN_MK: {
        // finalFunc = std::bind(funcPointer, MK, strictness, _1, _2);
        printf("building mediumKick\n");
        currentToken++;
      }
      break;
      case CTOKEN_NUMBER: {
        finalNode.bufferLength = strtol(currentToken->start, NULL, 10);
        currentToken++;
        printf("building or\n");
        finalFunc = binaryCommand(finalFunc, CTOKEN_OR);
        printf("or control returned\n");
      }
      break;
      case CTOKEN_AND: {
        currentToken++;
        printf("building and\n");
        finalFunc = binaryCommand(finalFunc, CTOKEN_AND);
      }
      break;
      default:
        break;
    }
  }

  if(currentToken->type != CTOKEN_END){
    currentToken++;
  }

  // CommandTokenType type;
  // const char* start;
  // uint8_t length;

  return finalNode;
}


CommandNode CommandCompiler::compileOneNode(){ return CommandNode(); }

CommandFunction CommandCompiler::binaryCommand(CommandFunction currentFunc, CommandTokenType type){ return CommandFunction();}
