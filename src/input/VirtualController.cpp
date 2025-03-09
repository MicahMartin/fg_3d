#include "VirtualController.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <bitset>
#include <sys/types.h>

VirtualController::VirtualController(){};
VirtualController::~VirtualController(){};

void VirtualController::update(uint16_t input){
  shiftHistory();
  InputEvent* eventFrame = inputHistory[0];
  int& currentEventCount = eventCounter[0];

  prevState = currentState;
  currentState = cleanSOCD(input);

  const uint16_t changedButtons = prevState ^ currentState;
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
    eventFrame[currentEventCount++] = InputEvent(mask, false);
    released ^= mask;
  }
}

inline bool VirtualController::isPressed(uint16_t input, bool strict) noexcept {
    const uint16_t direction_mask = 0x0F;
    const uint16_t current_dir = currentState & direction_mask;
    
    return ((input < 16) & strict) ? (current_dir == input) : (currentState & input);
}

inline bool VirtualController::wasPressed(uint16_t input, bool strict, int index, bool pressed) noexcept {
  if (index >= MAX_HISTORY || index < 0)
    return false;

  const InputEvent* eventList = inputHistory[index];
  const int eventCount = eventCounter[index];

  if (eventCount == 0)
    return false;

  const uint16_t strictMask = strict ? 0x0F : 0xFFFF;
  const uint16_t targetBits = (strict && input <= 10) ? (input) : (input & strictMask);

  for (int i = 0; i < eventCount; i++) {
    const InputEvent& event = eventList[i];
    if (event.valid && (event.pressed == pressed)) {
      const uint16_t eventBits = event.inputBit & strictMask;
      if (eventBits & targetBits) {
        return strict ? (eventBits == targetBits) : true;
      }
    }
  }
  return false;
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
  const uint16_t horizontal = input & Input::HORIZONTAL_OCD;
  if (horizontal == Input::HORIZONTAL_OCD)
    input &= ~Input::HORIZONTAL_OCD;

  const uint16_t vertical = input & Input::VERTICAL_OCD;
  if (vertical == Input::VERTICAL_OCD)
    input &= ~Input::VERTICAL_OCD;

  return input;
}
