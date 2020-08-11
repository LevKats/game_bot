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

    virtual uint32_t suggest(std::vector<std::vector> cells);

    virtual void inform(Answer);

    State get_state() const;

    friend Game;

private:
    State state;
};
