#include <stdexcept>
#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(){
  for (auto i = 0; i < MAX_HISTORY; i++) {
    buffer[i] = InputFrame{};
  }
}

void CircularBuffer::push(const InputFrame& elem){
  buffer[next] = elem;
  next = (next + 1) % MAX_HISTORY;
}

const InputFrame& CircularBuffer::front() const {
  return buffer[(next == 0 ? MAX_HISTORY : next) - 1];
}

InputFrame& CircularBuffer::operator[](int index){
  if (index >= MAX_HISTORY) 
    throw std::out_of_range("CircularBuffer out of range?");
  
  int head = (next == 0 ? MAX_HISTORY : next) - 1;
  return buffer[(head - index < 0) ? MAX_HISTORY + head - index : head - index];
};
