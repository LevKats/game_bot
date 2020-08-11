#pragma once
#include "Constants.h"
#include <string>
#include <vector>

class Game;

class Character {
public:
    using State = struct {
        bool alive;

        uint32_t ammos;

        uint32_t bombs;

        CharacterType type;

        std::string name;
    };

    Character();

    virtual ~Character();

    virtual uint32_t suggest(std::vector<std::vector<uint32_t>> cells) = 0;

    virtual void inform(uint32_t) = 0;

    State get_state() const;

    friend Game;

private:
    State state;
};
