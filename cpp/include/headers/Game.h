#pragma once
#include "Field.h"
#include "Logger.h"
#include "Player.h"
#include <vector>

class Game {
public:
    using PlayerFullState = struct {
        Character player;
        Field::Index index;
        std::vector<Field::Index> trace;
    };

    using GameSettings = struct {
        const uint32_t MAX_TRACE_LENGTH;
        const uint32_t HUMAN_FOV_RADIUS;
        const uint32_t BEAR_FOV_RADIUS;
        const uint32_t MAX_AMMOS;
        const uint32_t MAX_BOMBS;
    };

    Game(const std::vector<PlayerFullState> &,
         const std::vector<PlayerFullState> &, const Field &, Logger &,
         GameSettings);

    ~Game();

    void step();

    Field get_field() const;

    bool is_running() const;

    Player winner() const;

private:
    void _one_turn(PlayerFullState &);

    std::vector<PlayerFullState> bears;

    std::vector<PlayerFullState> humans;

    Field field;

    bool running;

    Logger &logger;

    const GameSettings settings;
};
