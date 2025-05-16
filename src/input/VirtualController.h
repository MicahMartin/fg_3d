#pragma once
#include <cstdint>
#include "CommandCompiler.h"
#include "CircularBuffer.h"

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
  bool checkCommand(int index, bool faceRight);
  std::string printHistory();

private:
  bool isPressed(uint32_t input, bool strict = true);
  bool wasPressed(uint32_t input, bool strict = true, bool pressed = true, int offset = 0);
  bool wasPressedBuffer(uint32_t input, bool strict = true, bool pressed = true, int buffLen = 2);
  bool evalPrefix(const std::vector<CommandIns>& code, int &ip, int &frameOffset);

  uint32_t cleanSOCD(uint32_t input);
  bool strictMatch(uint32_t bitsToCheck, uint32_t query);
  int findMatchingFrame(uint32_t operand, bool strict, bool pressed, int startOffset, int buffLen = 16);

  CommandCompiler commandCompiler;

  // stateful
  CircularBuffer<MAX_HISTORY, InputFrame> inputBuffer;
  uint32_t currentState{ 0 }, prevState{ 0 };
};
