#include "VirtualController.h"
#include <cstdio>

VirtualController::VirtualController(){};
VirtualController::~VirtualController(){};

void VirtualController::update(uint16_t input){
  prevState = currentState;
  currentState = input;
  InputEvent* eventFrame = inputHistory[historyIndex];

  uint16_t changedButtons = prevState ^ currentState;
  uint16_t pressed = changedButtons & currentState;
  uint16_t released = changedButtons & prevState;

  while (pressed) {
      uint16_t mask = pressed & -pressed; // Isolate lowest set bit
      // eventFrame.emplace_back(mask, true);
      pressed ^= mask; // Remove processed bit
  }

  while (released) {
      uint16_t mask = released & -released; 
      // eventFrame.emplace_back(mask, false);
      released ^= mask;
  }
}
