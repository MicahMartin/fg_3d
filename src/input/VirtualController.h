#pragma once
#include <cstdint>

constexpr int MAX_HISTORY{ 60 };
struct InputFrame {
  uint16_t pressedBits;
  uint16_t releasedBits;
  uint16_t validBits;
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
    HORIZONTAL_SOCD = (LEFT | RIGHT),
    VERTICAL_SOCD = (UP | DOWN),
    DIR_MASK = 0x000F,
    BTN_MASK = 0xFFF0
  };
}

class VirtualController {
public:
  VirtualController();
  VirtualController(VirtualController &&) = default;
  VirtualController(const VirtualController &) = default;
  VirtualController &operator=(VirtualController &&) = default;
  VirtualController &operator=(const VirtualController &) = default;
  ~VirtualController();

  void update(uint16_t input);
  bool isPressed(uint16_t input, bool strict = true);
  bool wasPressed(uint16_t input, bool strict = true, int index = 0, bool pressed = true);
  bool wasPressedBuffer(uint16_t input, bool strict = true, bool pressed = true);
private:
  void shiftHistory(); // We could use the old school ring buffer approach, but why? we flatten it every frame to serialize anyway
  uint16_t cleanSOCD(uint16_t input);
  bool strictMatch(uint16_t bitsToCheck, uint16_t query);

  InputFrame inputHistory[MAX_HISTORY];
  uint16_t currentState{ 0 };
  uint16_t prevState{ 0 };
};
