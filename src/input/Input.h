#pragma once
#include <cstdint>

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

