#include "VirtualController.h"
#include "CommandVm.h"
#include "Input.h"
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <sys/types.h>

VirtualController::VirtualController(){
  // for (int i=0; i < MAX_HISTORY; ++i) {
  //   inputHistory[i] = {
  //     .pressedBits = 0,
  //     .releasedBits = 0,
  //     .validBits = 0xFFFF,
  //   };
  // };

  commandCompiler.init("./char_def/commands.json");
};

VirtualController::~VirtualController(){};

void VirtualController::update(uint32_t input){
  // Clean and assign new current state
  prevState = currentState;
  currentState = cleanSOCD(input);

  // Extract button-only parts
  const uint32_t prevButtons = prevState & Input::BTN_MASK;
  const uint32_t currButtons = currentState & Input::BTN_MASK;
  const uint32_t changedButtons = prevButtons ^ currButtons;

  InputFrame currentFrame;
  currentFrame.pressedBits = changedButtons & currButtons;
  currentFrame.releasedBits = changedButtons & prevButtons;

  // Extract direction (stick) parts
  const uint32_t prevStick = prevState & Input::DIR_MASK;
  const uint32_t currStick = currentState & Input::DIR_MASK;

  if (prevStick != currStick) {
    currentFrame.pressedBits  |= currStick == 0 ? Input::NOINPUT : currStick;
    currentFrame.releasedBits |= prevStick == 0 ? Input::NOINPUT : prevStick;
  }

  inputBuffer.push(currentFrame);
}

bool VirtualController::isPressed(uint32_t input, bool strict) {
  return strict ? strictMatch(currentState, input) : (currentState & input) != 0;
}

bool VirtualController::wasPressed(uint32_t input, bool strict, bool pressed, int offset) {
  if (offset >= MAX_HISTORY || offset < 0) return false;

  const InputFrame& currentFrame = inputBuffer[offset];
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
    bool any = ins.operand & ANY_FLAG;
    bool negated = ins.operand & NOT_FLAG;
    uint32_t operand = ins.operand & 0x3FFFFFFF; // drop the 2 highest bits since we use them as flags
    bool result = false;  // Result for this instruction.
    int buffLen = 16;

    // printf("checking %s, operand:%d, strict:%d\n", commandCompiler.opcodeToString(ins.opcode).c_str(), ins.operand, strict);
    switch (ins.opcode) {
      case OP_PRESS: {
        int matchedFrame = findMatchingFrame(operand, !any, true, frameOffset, buffLen);
        result = (matchedFrame >= 0);
        if (result) {
          frameOffset = matchedFrame;
          // printf("found at offset %d\n", frameOffset);
        }
        break;
      }
      case OP_RELEASE: {
        int matchedFrame = findMatchingFrame(operand, !any, false, frameOffset, buffLen);
        result = (matchedFrame >= 0);
        if (result) {
          frameOffset = matchedFrame;
        }
        break;
      }
      case OP_HOLD: {
        // printf("Checking isPressed %d, %d\n", ins.operand, strict);
        result = isPressed(operand, !any);
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
          printf("HOLY FURK\n");
          break;
      }
      case OP_END: {
          // End-of-command marker: we finalize the overall result.
          // printf("we made it to the end, wtf? %d\n", overallResult);
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

std::string VirtualController::printHistory(){
  std::string retString;
  for (int i = 0; i < 8; i++) {
    retString += std::to_string(inputBuffer[i].pressedBits);
  }
  return retString;
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
