#pragma once
#include <cstdint>
#include "CommandCompiler.h"

constexpr int MAX_HISTORY{ 120 };

struct InputFrame {
  uint32_t pressedBits;
  uint32_t releasedBits;
  uint32_t validBits;
};

class VirtualController {
public:
  VirtualController();
  VirtualController(VirtualController &&) = default;
  VirtualController(const VirtualController &) = default;
  VirtualController &operator=(VirtualController &&) = default;
  VirtualController &operator=(const VirtualController &) = default;
  ~VirtualController();

  void update(uint32_t input);
  bool isPressed(uint32_t input, bool strict = true);
  bool wasPressed(uint32_t input, bool strict = true, bool pressed = true, int offset = 0);
  bool wasPressedBuffer(uint32_t input, bool strict = true, bool pressed = true, int buffLen = 2);
  bool checkCommand(int index, bool faceRight);

private:
  void shiftHistory(); // We could use the old school ring buffer approach, but why? we flatten it every frame to serialize anyway
  uint32_t cleanSOCD(uint32_t input);
  bool strictMatch(uint32_t bitsToCheck, uint32_t query);
  int findMatchingFrame(uint32_t operand, bool strict, bool pressed, int startOffset, int buffLen);

  uint32_t  currentState{ 0 }, 
            prevState{ 0 }, 
            noChangeCounter{ 0 };
  InputFrame inputHistory[MAX_HISTORY];
  CommandCompiler commandCompiler;
};
