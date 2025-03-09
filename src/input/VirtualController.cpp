#include "VirtualController.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <bitset>

VirtualController::VirtualController(){};
VirtualController::~VirtualController(){};

void VirtualController::update(uint16_t input){
  shiftHistory();
  InputEvent* eventFrame = inputHistory[0];
  int& currentEventCount = eventCounter[0];

  prevState = currentState;
  currentState = cleanSOCD(input);

  uint16_t changedButtons = prevState ^ currentState;
  uint16_t pressed = changedButtons & currentState;
  uint16_t released = changedButtons & prevState;

  // basically loops through an int's bitflags. cool pattern
  while (pressed && currentEventCount < MAX_EVENTS_PER_FRAME) {
    uint16_t mask = pressed & -pressed;
    eventFrame[currentEventCount++] = InputEvent(mask, true);
    pressed ^= mask;
  }

  while (released && currentEventCount < MAX_EVENTS_PER_FRAME) {
    uint16_t mask = released & -released; 
    eventFrame[eventCounter[0]++] = InputEvent(mask, false);
    released ^= mask;
  }
}

uint16_t VirtualController::getCurrentState(){
  return currentState;
}

void VirtualController::printHistory(){
  for (int frame = 0; frame < MAX_HISTORY; ++frame) {
    std::cout << "Frame " << frame << " (" << eventCounter[frame] << " events):\n";
    for (int event = 0; event < eventCounter[frame]; ++event) {
      const InputEvent& ie = inputHistory[frame][event];
      // Print the inputBit in binary form using std::bitset
      std::cout << "  Event " << event << ": " 
                << std::bitset<16>(ie.inputBit)
                << (ie.pressed ? " Pressed" : " Released") << "\n";
    }
  }
}

void VirtualController::shiftHistory(){
    static_assert(std::is_trivially_copyable_v<InputEvent>, "InputEvent must be trivially copyable");

    // Shift event counters
    std::memmove(eventCounter + 1, eventCounter, (MAX_HISTORY - 1) * sizeof(int));
    
    // Shift input history
    std::memmove(inputHistory + 1, inputHistory, (MAX_HISTORY - 1) * sizeof(inputHistory[0]));
    
    // Clear new frame
    eventCounter[0] = 0;
    std::memset(inputHistory[0], 0, sizeof(inputHistory[0]));
}

uint16_t VirtualController::cleanSOCD(uint16_t input){
  uint16_t horizontal = input & Input::HORIZONTAL_OCD;
  if (horizontal == Input::HORIZONTAL_OCD)
    input &= ~Input::HORIZONTAL_OCD;

  uint16_t vertical = input & Input::VERTICAL_OCD;
  if (vertical == Input::VERTICAL_OCD)
    input &= ~Input::VERTICAL_OCD;

  return input;
}
