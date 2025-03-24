#include "VirtualController.h"
#include <cstdint>
#include <cstring>
#include <ostream>
#include <sys/types.h>
#include <iostream>

VirtualController::VirtualController(){
  for (int i=0; i < MAX_HISTORY; ++i) {
    inputHistory[i] = {
      .pressedBits = 0,
      .releasedBits = 0,
      .validBits = 0xFFFF,
    };
  };
  commandCompiler.init("./char_def/commands.json", 
                       &VirtualController::wasPressedStatic,
                       &VirtualController::isPressedStatic,
                       this);
};

VirtualController::~VirtualController(){};

void VirtualController::update(uint16_t input){
  shiftHistory();

  prevState = currentState;
  currentState = cleanSOCD(input);

  const uint16_t changedButtons = prevState ^ currentState;

  InputFrame& currentFrame = inputHistory[0];
  currentFrame.pressedBits = changedButtons & currentState;
  currentFrame.releasedBits = changedButtons & prevState;
}

bool VirtualController::isPressed(uint16_t input, bool strict) {
  return strict ? strictMatch(currentState, input) : (currentState & input) != 0;
}

bool VirtualController::wasPressed(uint16_t input, bool strict, bool pressed, int index) {
  if (index >= MAX_HISTORY || index < 0) return false;

  const InputFrame& currentFrame = inputHistory[index];
  const uint16_t targetMask = pressed ? currentFrame.pressedBits : currentFrame.releasedBits;

  return strict ? strictMatch(targetMask, input) : (targetMask & input) != 0;
}

bool VirtualController::wasPressedBuffer(uint16_t input, bool strict, bool pressed, int buffLen){
  for (int i = 0; i < buffLen; i++) {
    if (wasPressed(input,strict,pressed,i)) return true;
  }
  return false;
}

bool VirtualController::checkCommand(int commandIndex, bool faceRight){
  return false;
}

void VirtualController::printHistory() {
  std::cout 
    << "\r"
    << ((currentState & Input::RIGHT)    ? "→  " : "-  ")  
    << ((currentState & Input::LEFT)     ? "←  " : "-  ")
    << ((currentState & Input::UP)       ? "↑  " : "-  ")
    << ((currentState & Input::DOWN)     ? "↓  " : "-  ")
    << ((currentState & Input::LIGHT_P)  ? "X  " : "-  ")  
    << ((currentState & Input::LIGHT_K)  ? "X  " : "-  ")
    << ((currentState & Input::MEDIUM_P) ? "X  " : "-  ")
    << ((currentState & Input::MEDIUM_K) ? "X  " : "-  ")
    << ((currentState & Input::HEAVY_P)  ? "X  " : "-  ")
    << ((currentState & Input::HEAVY_K)  ? "X  " : "-  ")
    << std::flush;
}

void VirtualController::shiftHistory(){
  std::memmove(&inputHistory[1], &inputHistory[0], sizeof(InputFrame) * (MAX_HISTORY - 1));

  inputHistory[0] = {
    .pressedBits = 0,
    .releasedBits = 0,
    .validBits = 0xFFFF  // All inputs start valid
  };
}

uint16_t VirtualController::cleanSOCD(uint16_t input){
  const uint16_t horizontal = input & Input::HORIZONTAL_SOCD;
  if (horizontal == Input::HORIZONTAL_SOCD)
    input &= ~Input::HORIZONTAL_SOCD;

  const uint16_t vertical = input & Input::VERTICAL_SOCD;
  if (vertical == Input::VERTICAL_SOCD)
    input &= ~Input::VERTICAL_SOCD;

  return input;
}

bool VirtualController::strictMatch(uint16_t bitsToCheck, uint16_t query) {
  // Extract the directional and button parts from the query.
  const uint16_t queryDir = query & Input::DIR_MASK;
  const uint16_t queryBtn = query & Input::BTN_MASK;

  // Check directional bits if any were provided.
  bool dirMatch = (queryDir == 0) || ((bitsToCheck & Input::DIR_MASK) == queryDir);
  // Check button bits if any were provided.
  bool btnMatch = (queryBtn == 0) || ((bitsToCheck & Input::BTN_MASK) == queryBtn);

  return dirMatch && btnMatch;
}

static bool wasPressedStatic(uint16_t key, bool repeat, int delay, bool held, void* ctx) {
    return static_cast<VirtualController*>(ctx)->wasPressed(key, repeat, delay, held);
}

static bool isPressedStatic(uint16_t key, bool repeat, void* ctx) {
    return static_cast<VirtualController*>(ctx)->isPressed(key, repeat);
}
