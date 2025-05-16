#pragma once
#include <cstdio>
#include <cstdlib>

template <int N, typename T>
class CircularBuffer {
public:
  CircularBuffer(){
    for (auto i = 0; i < N; i++) {
      buffer[i] = T{};
    }
  }

  void push(const T& elem){
    buffer[next] = elem;
    next++;
    next = next % N;
  }

  const T& front() const {
    int i = (next == 0) ? N : next;
    return buffer[--i];
  }

  T& operator[](int index){
    if (index >= N) {
      printf("CircularBuffer Index out of bounds\n");
      exit(0);
    }
    int head = ((next == 0) ? N : next) - 1;
    if(head - index < 0){
      return buffer[N + (head-index)];
    }
    return buffer[head - index];
  };

  ~CircularBuffer(){};

private:
  T buffer[N];
  int next = 0;
};
