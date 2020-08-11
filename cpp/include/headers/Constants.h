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

enum CommandFlags : uint32_t {
    PASS = 1,
    GO = 2,
    TRY_EXIT = 4,
    SHOOT = 8,
    EXPLODE = 16,

    LEFT = 32,
    RIGHT = 64,
    UP = 128,
    DOWN = 256,

    BEAR_ATACK = 512
};

enum Answer { SUCCESS = 0, NOTHING = 1, UNK = 2, ERR = 3 };

enum CharacterType { HUMAN = 0, BEAR = 1 };
