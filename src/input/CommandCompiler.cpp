#include "CommandCompiler.h"
#include "CommandScanner.h"
#include "CommandVm.h"
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <glaze/glaze.hpp>
#include <glaze/json/read.hpp>
#include <iostream>

CommandCompiler::CommandCompiler(){}

CommandCompiler::~CommandCompiler(){}

void CommandCompiler::init(const char* path) {
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

const CommandCode* CommandCompiler::getCommand(int index) const {
  if (index < 0 || index >= commands.size())
    throw std::runtime_error("trying to access out of bounds command");

  return &commands[index];
}

std::string CommandCompiler::opcodeToString(CommandOp opcode) {
  switch (opcode) {
    case OP_PRESS:    return "OP_PRESS";
    case OP_RELEASE:  return "OP_RELEASE";
    case OP_HOLD:     return "OP_HOLD";
    case OP_DELAY:    return "OP_DELAY";
    case OP_AND:      return "OP_AND";
    case OP_OR:       return "OP_OR";
    case OP_END:      return "OP_END";
    default:          return "UNKNOWN_OP";
  }
}

void CommandCompiler::printCode(const CommandCode& command) {
  std::cout << "=== Command Bytecode ===\n";
  
  for (const auto& instruction : command.instructions) {
    std::cout << std::setw(12) << std::left << opcodeToString(instruction.opcode)
              << " Operand: 0x" << std::hex << std::setw(4) << std::setfill(' ') << instruction.operand
              << " (";

    // Extract input mask and modifier flags
    bool isNonStrict = instruction.operand & ANY_FLAG;
    bool isNegated = instruction.operand & NOT_FLAG;

    // Print extracted components
    if (isNonStrict) std::cout << " @";
    if (isNegated) std::cout << " !";
    
    std::cout << ")\n";
  }

  std::cout << "========================\n";
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
  // Mark the end of the command (at the start, since we reverse - iterate the bytecode.
  std::reverse(code.instructions.begin(), code.instructions.end());
  code.instructions.push_back({ OP_END, 0 });

  commands.push_back(code);
  printf("Compiled command: %s\n", inputString);
  printCode(code);
}

// This function compiles a sequence of tokens into a CommandBytecode.
CommandCode CommandCompiler::compileNode() {
  CommandCode bytecode;
  std::vector<CommandToken> expression;
  std::vector<CommandIns> outputQueue;
  std::vector<CommandOp>  opStack;

  auto precedence = [&](CommandTokenType t) {
    switch (t) {
      case CTOKEN_AND: return 2;
      case CTOKEN_OR:  return 1;
      default:         return 0;
    }
  };

  auto flushOps = [&]() {
    while (!opStack.empty()) {
      outputQueue.push_back({ opStack.back(), 0 });
      opStack.pop_back();
    }
  };
  bool any = false,
  negate = false,
  held = false,
  release = false;
  // Process tokens until we hit a delimiter (CTOKEN_DELIM) or the end token.
  while (currentToken->type != CTOKEN_DELIM && currentToken->type != CTOKEN_END) {
    expression.push_back(*currentToken);
    currentToken++;
  }

  for (const CommandToken &tok : expression) {
    switch (tok.type) {
      case CTOKEN_ANY:
        any = true;
        break;
      case CTOKEN_NOT:
        negate = true;
        break;
      case CTOKEN_HELD:
        held = true;
        break;
      case CTOKEN_RELEASED:
        release = true;
        break;
      case CTOKEN_NEUTRAL: case CTOKEN_FORWARD: case CTOKEN_BACK:
      case CTOKEN_UP: case CTOKEN_DOWN: case CTOKEN_UPFORWARD:
      case CTOKEN_UPBACK: case CTOKEN_DOWNFORWARD: case CTOKEN_DOWNBACK:
      case CTOKEN_LP: case CTOKEN_LK: case CTOKEN_MP: case CTOKEN_MK: {
        // NOTE: simpler to set flags as you scan, but here we assume
        //       that parser already kept modifiers immediately before.
        // --- determine opcode:
        CommandOp op = OP_PRESS;
        printf("BRO WHAT DE FUK?? any:%d, negate:%d, held:%d, release:%d\n", any, negate, held, release);
        // if (held)    op = OP_HOLD;
        if (release) op = OP_RELEASE;
        // --- build final mask:
        uint32_t mask = parseInputMask(&tok);
        if (any)    mask |= ANY_FLAG;
        if (negate)   mask |= NOT_FLAG;

        // --- push the operand instruction
        outputQueue.push_back({ op, mask });
        any = false;
        negate = false;
        held = false;
        release = false;
        break;
      }
      // — OPERATORS: handle '&' and '|' by precedence
      case CTOKEN_AND:
      case CTOKEN_OR: {
        CommandOp thisOp = (tok.type == CTOKEN_AND ? OP_AND : OP_OR);
        int thisPrec    = precedence(tok.type);

        // pop any operators of >= precedence
        while (!opStack.empty()) {
          // need inverse‑map from CommandOp back to token type for precedence
          CommandOp topOp = opStack.back();
          CommandTokenType topTok = (topOp == OP_AND ? CTOKEN_AND : topOp == OP_OR  ? CTOKEN_OR : CTOKEN_END);
          if (precedence(topTok) >= thisPrec) {
            outputQueue.push_back({ topOp, 0 });
            opStack.pop_back();
          } else {
            break;
          }
        }
        // push the new operator
        opStack.push_back(thisOp);
        break;
      }

      default:
        // ignore ANY, NOT, HELD, RELEASED here — assume folded into operands
        break;
    }
  }
  // 5) end‑of‑subexpr: flush any remaining operators into output
  flushOps();

  // 6) append postfix instructions into the returned CommandCode
  bytecode.instructions = std::move(outputQueue);

  return bytecode;
}
