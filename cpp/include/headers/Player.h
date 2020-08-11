#pragma once
#include "Character.h"
#include <istream>
#include <ostream>

class Player : public Character {
public:
    Player(std::istream &in, std::ostream &out);

    ~Player();

    uint32_t suggest(std::vector<std::vector> cells) override;

    void inform(Answer) override;

private:
    std::istream &in;

    std::ostream &out;
};
