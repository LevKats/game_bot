#pragma once
#include <cstdint>

enum CellFlags : uint32_t {
    HIDDEN = 1,

    CHARACTER = 2,
    TRACE = 4,

    LEFT_BORDER = 8,
    RIGHT_BORDER = 16,
    UP_BORDER = 32,
    DOWN_BORDER = 64,

    HOSPITAL = 128,
    ARMORY = 256,

    WORM_HOLL = 512,

    EXIT = 1024,
    TREASURE = 2048
};

enum Command : uint32_t {
    PASS = 0,
    GO = 1,
    TRY_EXIT = 2,
    SHOOT = 3,
    EXPLODE = 4,
    BEAR_ATACK = 5,
    COMMAND_MASK = 1 + 2 + 4,

    LEFT = 8 * 0,
    RIGHT = 8 * 1,
    UP = 8 * 2,
    DOWN = 8 * 3,
    NONE = 8 * 4,
    DIRECTION_MASK = 8 * (1 + 2 + 4)
};

enum Answer : uint32_t {
    SUCCESS = 0,
    NOTHING = 1,
    UNK = 2,
    ERR = 3,
    STATUS_MASK = 1 + 2,

    HOLL_MASK = 4
};

enum CharacterType { HUMAN = 0, BEAR = 1 };
