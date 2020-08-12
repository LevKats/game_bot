#include "Game.h"
#include <algorithm>

Game::Game(const std::vector<Game::PlayerFullState> &bears,
           const std::vector<Game::PlayerFullState> &humans,
           const Field &field, Logger &logger, GameSettings settings)
    : bears(bears), humans(humans), field(field), running(true),
      logger(logger), settings(settings) {
    for (auto &bear : bears) {
        bear.player.state = {true, 0, 0, CharacterType::BEAR,
                             bear.get_state().name};
    }
    for (auto &haman : humans) {
        human.player.state = {true, settings.MAX_AMMOS, settings.MAX_BOMBS,
                              CharacterType::HUMAN, human.get_state().name};
    }
}

Game::~Game() {}

void Game::_one_turn(Game::PlayerFullState &player) {
    Field field_copy_atacking(field);
    Field field_copy_suggest(field);
    logger.log(std::string("turn of player with name") + player.player.name);

    // TODO
}

void Game::step() {
    // TODO
}

Field Game::get_field() const { return field; }

bool Game::is_running() const { return running; }

Player Game::winner() const {
    if (is_running) {
        throw std::runtime_error("the game still running");
    }
    if (humans.size() == 1) {
        return human[0];
    }
    throw std::runtime_error("no winners somehow");
}
