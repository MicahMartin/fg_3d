#include "VirtualController.h"
#include <cstdint>
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

bool VirtualController::wasPressed(uint16_t input, bool strict, int index, bool pressed) {
  if (index >= MAX_HISTORY || index < 0) return false;

  const InputFrame& currentFrame = inputHistory[index];
  const uint16_t targetMask = pressed ? currentFrame.pressedBits : currentFrame.releasedBits;

  return strict ? strictMatch(targetMask, input) : (targetMask & input) != 0;
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
