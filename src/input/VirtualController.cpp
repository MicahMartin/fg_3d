#include "VirtualController.h"
#include <cstdint>
#include <cstring>
#include <sys/types.h>

VirtualController::VirtualController(){};
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
    const uint16_t direction_mask = 0x0F;
    const uint16_t current_dir = currentState & direction_mask;
    
    return ((input < 16) & strict) ? (current_dir == input) : (currentState & input);
}

bool VirtualController::wasPressed(uint16_t input, bool strict, int index, bool pressed) {
  if (index >= MAX_HISTORY || index < 0) return false;

  const InputFrame& currentFrame = inputHistory[index];
  const uint16_t targetMask = pressed ? currentFrame.pressedBits : currentFrame.releasedBits;

  if (strict) {
    return ((targetMask & input) == input) && ((currentFrame.validBits & input) == input);
  } else {
    return (targetMask & input & currentFrame.validBits) != 0;
  }
}

uint16_t VirtualController::getCurrentState(){
  return currentState;
}

void VirtualController::printHistory(){
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
  const uint16_t horizontal = input & Input::HORIZONTAL_OCD;
  if (horizontal == Input::HORIZONTAL_OCD)
    input &= ~Input::HORIZONTAL_OCD;

  const uint16_t vertical = input & Input::VERTICAL_OCD;
  if (vertical == Input::VERTICAL_OCD)
    input &= ~Input::VERTICAL_OCD;

  return input;
}
