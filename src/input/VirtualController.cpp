#include "VirtualController.h"
#include "CommandVm.h"
#include "Input.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sys/types.h>

VirtualController::VirtualController(){
  for (int i=0; i < MAX_HISTORY; ++i) {
    inputHistory[i] = {
      .pressedBits = 0,
      .releasedBits = 0,
      .validBits = 0xFFFF,
    };
  };

  commandCompiler.init("./char_def/commands.json");
};

VirtualController::~VirtualController(){};

void VirtualController::update(uint32_t input){
  shiftHistory();

  prevState = currentState;
  currentState = cleanSOCD(input);

  const uint32_t changedButtons = prevState ^ currentState;

  InputFrame& currentFrame = inputHistory[0];
  currentFrame.pressedBits = changedButtons & currentState;
  currentFrame.releasedBits = changedButtons & prevState;

  // Extract cardinal (directional) parts:
  uint32_t prevStickState = prevState & Input::DIR_MASK;
  uint32_t currentStickState = currentState & Input::DIR_MASK;

  if (prevStickState != currentStickState) {
    if (currentStickState == 0) {
      currentFrame.pressedBits |= Input::NOINPUT;
    }   
  }
}

bool VirtualController::isPressed(uint32_t input, bool strict) {
  return strict ? strictMatch(currentState, input) : (currentState & input) != 0;
}

bool VirtualController::wasPressed(uint32_t input, bool strict, bool pressed, int offset) {
  if (offset >= MAX_HISTORY || offset < 0) return false;

  const InputFrame& currentFrame = inputHistory[offset];
  const uint32_t targetMask = pressed ? currentFrame.pressedBits : currentFrame.releasedBits;

  return strict ? strictMatch(targetMask, input) : (targetMask & input) != 0;
}

bool VirtualController::wasPressedBuffer(uint32_t input, bool strict, bool pressed, int buffLen){
  for (int i = 0; i < buffLen; i++) {
    if (wasPressed(input,strict,pressed,i)) return true;
  }
  return false;
}

bool VirtualController::checkCommand(int index, bool faceRight) {
  const CommandCode* code = commandCompiler.getCommand(index);
  int frameOffset = 0;       // Start checking from the most recent frame.
  bool overallResult = true; // This will accumulate the results of all instructions.

  // Iterate over each instruction in the command's bytecode.seth rogan
  for (const auto& ins : code->instructions) {
    bool strict = ins.opcode & STRICT_FLAG;
    bool negated = ins.opcode & NOT_FLAG;
    bool result = false;  // Result for this instruction.
    int buffLen = 16;

    // printf("checking %s, operand:%d, strict:%d\n", commandCompiler.opcodeToString(ins.opcode).c_str(), ins.operand, strict);
    switch (ins.opcode) {
      case OP_PRESS: {
        int matchedFrame = findMatchingFrame(ins.operand, strict, true, frameOffset, buffLen);
        result = (matchedFrame >= 0);
        if (result) {
          frameOffset = matchedFrame;
          printf("found at offset %d\n", frameOffset);
        }
        break;
      }
      case OP_RELEASE: {
        int matchedFrame = findMatchingFrame(ins.operand, strict, false, frameOffset, buffLen);
        result = (matchedFrame >= 0);
        if (result) {
          frameOffset = matchedFrame;
        }
        break;
      }
      case OP_HOLD: {
        printf("Checking isPressed %d, %d\n", ins.operand, strict);
        result = isPressed(ins.operand, strict);
        break;
      }
      case OP_AND: {
        result = true;
        break;
      }
      case OP_OR: {
          // For OR, a proper implementation would combine alternate paths.
          // As a placeholder, we'll assume the branch passes if overallResult is true.
          result = true;
          break;
      }
      case OP_END: {
          // End-of-command marker: we finalize the overall result.
          printf("we made it to the end, wtf? %d\n", overallResult);
          return overallResult;
      }
      default: {
          // Unrecognized opcode results in failure.
          result = false;
          break;
      }
    }
    // Combine the result of this instruction into the overall result.
    overallResult = overallResult && result;
    // If at any point the overall result is false, we can stop early.
    if (!overallResult) return false;
  }
  return overallResult;
}

void VirtualController::shiftHistory(){
  std::memmove(&inputHistory[1], &inputHistory[0], sizeof(InputFrame) * (MAX_HISTORY - 1));

  inputHistory[0] = {
    .pressedBits = 0,
    .releasedBits = 0,
    .validBits = 0xFFFF  // All inputs start valid
  };
}

uint32_t VirtualController::cleanSOCD(uint32_t input){
  const uint32_t horizontal = input & Input::HORIZONTAL_SOCD;
  if (horizontal == Input::HORIZONTAL_SOCD)
    input &= ~Input::HORIZONTAL_SOCD;

  const uint32_t vertical = input & Input::VERTICAL_SOCD;
  if (vertical == Input::VERTICAL_SOCD)
    input &= ~Input::VERTICAL_SOCD;

  return input;
}

bool VirtualController::strictMatch(uint32_t bitsToCheck, uint32_t query) {
  // Extract the directional and button parts from the query.
  const uint32_t queryDir = query & Input::DIR_MASK;
  const uint32_t queryBtn = query & Input::BTN_MASK;

  // Check directional bits if any were provided.
  bool dirMatch = (queryDir == 0) || ((bitsToCheck & Input::DIR_MASK) == queryDir);
  // Check button bits if any were provided.
  bool btnMatch = (queryBtn == 0) || ((bitsToCheck & Input::BTN_MASK) == queryBtn);

  return dirMatch && btnMatch;
}

int VirtualController::findMatchingFrame(uint32_t operand, bool strict, bool pressed, int startOffset, int buffLen){
  for (int i = startOffset; i < buffLen; ++i) {
    if (wasPressed(operand, strict, pressed, i)) 
      return i;
  }
  return -1;
}
