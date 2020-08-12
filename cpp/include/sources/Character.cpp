#include "Character.h"

Character::Character() {}

Character::Character(const Character &c) : state(c.state) {}

Character::~Character() {}

void Character::set_name(std::string name) { state.name = name; }

Character::State Character::get_state() const { return state; }
