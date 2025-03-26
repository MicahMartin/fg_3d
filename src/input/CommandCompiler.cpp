#include "CommandCompiler.h"
#include "CommandVm.h"

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
  // Create a new CommandCode to hold the bytecode instructions.
  CommandCode code;
  code.clears = clears;

  std::vector<CommandToken> tokens = commandScanner.scan(inputString);
  currentToken = &tokens[0];

  // Loop until we reach the end token.
  while (currentToken->type != CTOKEN_END) {
    // Compile a sub-node into a block of instructions.
    CommandCode subCode = compileNode();
    // Append the subnode instructions into our overall code.
    code.instructions.insert(code.instructions.end(), 
                               subCode.instructions.begin(), 
                               subCode.instructions.end());
    // If the next token is a delimiter, skip it.
    if (currentToken->type == CTOKEN_DELIM) {
      currentToken++;
    }
  }
  commands.push_back(code);
  printf("Compiled command: %s\n", inputString);
}

// This function compiles a sequence of tokens into a CommandBytecode.
CommandCode CommandCompiler::compileNode() {
  CommandCode bytecode;
  
  std::vector<CommandIns> instructions;
  // Local modifier flags to apply only to the next input token.
  bool nonStrict = false;    // Set by CTOKEN_ANY (@)
  bool releaseFlag = false;  // Set by CTOKEN_RELEASED (~)
  bool heldFlag = false;     // Set by CTOKEN_HELD (*)
  bool notFlag = false;      // Set by CTOKEN_NOT (!)
  
  // Process tokens until we hit a delimiter (CTOKEN_DELIM) or the end token.
  while (currentToken->type != CTOKEN_DELIM && currentToken->type != CTOKEN_END) {
    switch (currentToken->type) {
      case CTOKEN_ANY:
        nonStrict = true;
        currentToken++;
        break;
      case CTOKEN_RELEASED:
        releaseFlag = true;
        currentToken++;
        break;
      case CTOKEN_HELD:
        heldFlag = true;
        currentToken++;
        break;
      case CTOKEN_NOT:
        notFlag = true;
        currentToken++;
        break;
      // For actual input tokens.
      case CTOKEN_NEUTRAL:
      case CTOKEN_FORWARD:
      case CTOKEN_BACK:
      case CTOKEN_UP:
      case CTOKEN_DOWN:
      case CTOKEN_LP:
      case CTOKEN_LK:
      case CTOKEN_MP:
      case CTOKEN_MK: {
        // Get the base input mask from the token.
        uint16_t baseMask = parseInputMask(currentToken);
        
        // Choose the opcode based on modifier flags.
        CommandOp opcode = OP_PRESS; // Default is a press check.
        if (heldFlag) {
            opcode = OP_HOLD;
        } else if (releaseFlag) {
            opcode = OP_RELEASE;
        }
        // Construct the final operand: combine base mask with modifier flags.
        uint16_t finalOperand = baseMask;
        if (nonStrict) {
            finalOperand |= NONSTRICT_FLAG;
        }
        if (notFlag) {
            finalOperand |= NOT_FLAG;
        }
        
        // Emit the instruction.
        bytecode.instructions.push_back({ opcode, finalOperand });
        
        // Reset modifier flags (they apply only to this token).
        nonStrict = false;
        releaseFlag = false;
        heldFlag = false;
        notFlag = false;
        
        currentToken++;
        break;
      }
      case CTOKEN_NUMBER: {
        // A numeric token indicates a delay (timing constraint).
        uint16_t delay = parseNumber(currentToken);
        bytecode.instructions.push_back({ OP_DELAY, delay });
        currentToken++;
        break;
      }
      // Handle binary operators.
      case CTOKEN_AND: {
        bytecode.instructions.push_back({ OP_AND, 0 });
        currentToken++;
        break;
      }
      case CTOKEN_OR: {
        bytecode.instructions.push_back({ OP_OR, 0 });
        currentToken++;
        break;
      }
      default: {
        // Skip any unexpected tokens.
        currentToken++;
        break;
      }
    }
  }
  
  // Mark the end of the command.
  bytecode.instructions.push_back({ OP_END, 0 });
  return bytecode;
}
