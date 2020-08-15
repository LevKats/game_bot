#pragma once
#include "Field.h"
#include "Logger.h"
#include "Player.h"
#include <vector>

class Game {
public:
    using PlayerFullState = struct {
        std::shared_ptr<Character> player;
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
         const std::vector<PlayerFullState> &, const Field &,
         std::shared_ptr<Logger> logger, GameSettings);

    ~Game();

    void step();

    const Field &get_field() const;

    bool is_running() const;

    std::shared_ptr<Character> winner();

private:
    bool _one_turn(std::vector<PlayerFullState>::iterator);

    uint32_t _end_turn(Field::Index &i, std::vector<Field::Index> &trace,
                       Character::State &state);

    void _process_point(Field::Index i, Character::State &state);

    std::vector<PlayerFullState> bears;

    std::vector<PlayerFullState> humans;

    Field field;

    bool running;

    std::shared_ptr<Logger> logger;

    const GameSettings settings;
};
