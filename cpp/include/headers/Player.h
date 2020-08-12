#pragma once
#include "Character.h"
#include <functional>

class Player : public Character {
public:
    Player(std::function<std::string()> in,
           std::function<void(std::string)> out, std::string name);

    Player(const Player &);

    ~Player();

    uint32_t suggest(std::vector<std::vector<uint32_t>> cells) override;

    void inform(uint32_t) override;

private:
    std::function<std::string()> in;

    std::function<void(std::string)> out;
};
