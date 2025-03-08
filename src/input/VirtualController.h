#pragma once

#include <cstdint>

struct InputEvent {
  InputEvent(uint16_t inputBit, bool pressed): inputBit(inputBit), pressed(pressed){}
  InputEvent(){}
  ~InputEvent(){}

  uint16_t inputBit = -1;
  bool pressed = 0;
  bool valid = true;
};

class VirtualController {
public:
  VirtualController();
  VirtualController(VirtualController &&) = default;
  VirtualController(const VirtualController &) = default;
  VirtualController &operator=(VirtualController &&) = default;
  VirtualController &operator=(const VirtualController &) = default;
  ~VirtualController();

  void update(uint16_t input);
private:
  InputEvent inputHistory[120][8];
  uint16_t currentState;
  uint16_t prevState;
  int eventCounter[120] = {0};
  int historyIndex = 0;
};
