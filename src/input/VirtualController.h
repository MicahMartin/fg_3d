#pragma once
#include <cstdint>

constexpr int MAX_HISTORY{ 60 };
constexpr int MAX_EVENTS_PER_FRAME{ 16 };

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
  void printHistory();
  uint16_t getCurrentState();

private:
  void shiftHistory(); // We could use the old school ring buffer approach, 
                       // but why? we flatten it every frame to serialize anyway
  uint16_t cleanSOCD(uint16_t input);
  InputEvent inputHistory[MAX_HISTORY][MAX_EVENTS_PER_FRAME];
  int eventCounter[MAX_HISTORY] = {0};
  int historyIndex{ 0 };
  uint16_t currentState{ 0 };
  uint16_t prevState{ 0 };
};

namespace Input {
  enum InputMasks : uint16_t {
    NOINPUT = 0,

    RIGHT = 0x1,
    LEFT = 0x2,
    UP = 0x4,
    DOWN = 0x8,

    LIGHT_P  = 0x10,
    MEDIUM_P = 0x20,
    HEAVY_P = 0x40,
    ALL_P = 0x80,

    LIGHT_K  = 0x100,
    MEDIUM_K = 0x200,
    HEAVY_K = 0x400,
    ALL_K = 0x800,

    START = 0x1000,
    SELECT = 0x2000,
    MISC1 = 0x4000,
    MISC2 = 0x8000,

    DOWNLEFT = (DOWN | LEFT),
    DOWNRIGHT = (DOWN | RIGHT),
    UPLEFT = (UP | LEFT),
    UPRIGHT = (UP | RIGHT),
    HORIZONTAL_OCD = (LEFT | RIGHT),
    VERTICAL_OCD = (UP | DOWN),
  };
}
