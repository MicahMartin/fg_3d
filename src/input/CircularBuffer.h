#pragma once
#include <cstdint>
#include <vector>

class CircularBuffer {
public:
  CircularBuffer();
  CircularBuffer(CircularBuffer &&) = default;
  CircularBuffer(const CircularBuffer &) = default;
  CircularBuffer &operator=(CircularBuffer &&) = default;
  CircularBuffer &operator=(const CircularBuffer &) = default;
  ~CircularBuffer();

private:
  std::vector<uint32_t> body;
};
