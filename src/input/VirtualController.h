#pragma once
#include <cstdint>

constexpr int MAX_HISTORY = 120;
constexpr int MAX_EVENTS_PER_FRAME = 16;

struct InputEvent {
  uint16_t inputBit = -1;
  bool pressed = true;
  bool valid = true;

  InputEvent(uint16_t inputBit, bool pressed) : inputBit(inputBit), pressed(pressed) {}
  InputEvent(){}
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

  // getters
  uint16_t getCurrentState();
private:
  uint16_t currentState;
  uint16_t prevState;

  InputEvent inputHistory[MAX_HISTORY][MAX_EVENTS_PER_FRAME];
  int eventCounter[MAX_HISTORY] = {0};
  int historyIndex = 0;
};

typedef enum {
  NOINPUT = 0,

  RIGHT = 0x1,
  LEFT = 0x2,
  UP = 0x4,
  DOWN = 0x8,

  LP  = 0x10,
  MP = 0x20,
  HP = 0x40,
  AP = 0x80,

  LK  = 0x100,
  MK = 0x200,
  HK = 0x400,
  AK = 0x800,

  START = 0x1000,
  SELECT = 0x2000,
  MISC1 = 0x4000,
  MISC2 = 0x8000,

  DOWNLEFT = (DOWN | LEFT),
  DOWNRIGHT = (DOWN | RIGHT),
  UPLEFT = (UP | LEFT),
  UPRIGHT = (UP | RIGHT),
} Input;
