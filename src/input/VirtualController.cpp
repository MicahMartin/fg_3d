#include "VirtualController.h"

VirtualController::VirtualController(){};
VirtualController::~VirtualController(){};

void VirtualController::update(uint16_t input){
  eventCounter[historyIndex] = 0;
  InputEvent* eventFrame = inputHistory[historyIndex];

  prevState = currentState;
  currentState = input;

  uint16_t changedButtons = prevState ^ currentState;
  uint16_t pressed = changedButtons & currentState;
  uint16_t released = changedButtons & prevState;

  while (pressed) {
    uint16_t mask = pressed & -pressed;
    if (eventCounter[historyIndex] < MAX_EVENTS_PER_FRAME) {
      eventFrame[eventCounter[historyIndex]++] = InputEvent(mask, true);
    }
    pressed ^= mask;
  }

  while (released) {
    uint16_t mask = released & -released; 
    if (eventCounter[historyIndex] < MAX_EVENTS_PER_FRAME) {
      eventFrame[eventCounter[historyIndex]++] = InputEvent(mask, false);
    }
    released ^= mask;
  }

  historyIndex = (historyIndex + 1) % MAX_HISTORY;
}

uint16_t VirtualController::getCurrentState(){
  return currentState;
}
