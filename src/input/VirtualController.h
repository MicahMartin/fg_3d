#pragma once
#include <cstdint>
#include "CommandCompiler.h"

constexpr int MAX_HISTORY{ 60 };

struct InputFrame {
  uint16_t pressedBits;
  uint16_t releasedBits;
  uint16_t validBits;
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
  bool isPressed(uint16_t input, bool strict = true);
  bool wasPressed(uint16_t input, bool strict = true, bool pressed = true, int index = 0);
  bool wasPressedBuffer(uint16_t input, bool strict = true, bool pressed = true, int buffLen = 2);
  bool checkCommand(int commandIndex, bool faceRight);

  void printHistory();

private:
  void shiftHistory(); // We could use the old school ring buffer approach, but why? we flatten it every frame to serialize anyway
  uint16_t cleanSOCD(uint16_t input);
  bool strictMatch(uint16_t bitsToCheck, uint16_t query);

  CommandCompiler commandCompiler;
  InputFrame inputHistory[MAX_HISTORY];
  uint16_t currentState{ 0 };
  uint16_t prevState{ 0 };
  int noChangeCounter{ 0 };

  static inline bool wasPressedStatic(void* ctx, uint16_t input, bool strict = true, bool pressed = true, int index = 0) {
    return static_cast<VirtualController*>(ctx)->wasPressed(input, strict, pressed, index);
  }

  static inline bool isPressedStatic(void* ctx, uint16_t input, bool strict = true) {
    return static_cast<VirtualController*>(ctx)->isPressed(input, strict);
  }

};
