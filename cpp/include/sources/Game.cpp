#include "Game.h"
#include "JSON.h"
#include <algorithm>
#include <functional>
#include <map>

Game::Game(const std::vector<Game::PlayerFullState> &bears_,
           const std::vector<Game::PlayerFullState> &humans_,
           const Field &field, std::shared_ptr<Logger> logger,
           GameSettings settings)
    : bears(bears_), humans(humans_), field(field), running(true),
      logger(logger), settings(settings), _winner(nullptr) {
    for (auto &bear : bears) {
        auto &state = bear.player->state;
        state = {true, false, 0, 0, CharacterType::BEAR, state.name};
    }
    for (auto &human : humans) {
        auto &state = human.player->state;
        state = {true,
                 false,
                 settings.MAX_AMMOS,
                 settings.MAX_BOMBS,
                 CharacterType::HUMAN,
                 state.name};
    }
}

Game::~Game() {}

uint32_t Game::_end_turn(Field::Index &i, std::vector<Field::Index> &trace,
                         Character::State &state) {
    for (int count = 0;; ++count) {
        if (count) {
            if (field[i] & CellFlags::WORM_HOLL) {
                i = field.next_wormholl(i);
            } else {
                break;
            }
        }
        _process_point(i, state);
        trace.push_back(i);
        while (trace.size() > settings.MAX_TRACE_LENGTH) {
            trace.erase(trace.begin());
        }
        if (count) {
            return Answer::HOLL_MASK;
        }
    }
    return 0;
}

void Game::_process_point(Field::Index i, Character::State &state) {
    if (field[i] & CellFlags::TREASURE && state.type == CharacterType::HUMAN &&
        !state.treasure && state.alive) {
        state.treasure = true;
        field[i] ^= CellFlags::TREASURE;
    }
    if (field[i] & CellFlags::ARMORY && state.type == CharacterType::HUMAN &&
        state.alive) {
        state.ammos = settings.MAX_AMMOS;
        state.bombs = settings.MAX_BOMBS;
    }
    if (field[i] & CellFlags::HOSPITAL && state.type == CharacterType::HUMAN) {
        state.alive = true;
    }
}

bool Game::_one_turn(std::vector<Game::PlayerFullState>::iterator it) {
    auto &player = *(it->player);
    auto &state = it->player->state;
    auto &index = it->index;
    auto &trace = it->trace;

    Field suggest(field);
    Field atacking(field);

    Field debug(field);
    {
        for (auto &human : humans) {
            debug[human.index] |= CellFlags::CHARACTER;
            for (auto ind : human.trace) {
                debug[ind] |= CellFlags::TRACE;
                debug[ind] |= CellFlags::TRACE;
            }
        }
        for (auto &bear : bears) {
            debug[bear.index] |= CellFlags::CHARACTER;
            for (auto ind : bear.trace) {
                debug[ind] |= CellFlags::TRACE;
                debug[ind] |= CellFlags::TRACE;
            }
        }
    }
    debug[index] |= CellFlags::CURSOR;
    logger->log("\n" + debug.render());

    if (state.type == CharacterType::HUMAN) {
        for (auto it2 = humans.begin(); it2 != humans.end(); ++it2) {
            if (it2 != it && it2->player->state.alive) {
                atacking[it2->index] |= CellFlags::CHARACTER;
            }
        }
        for (auto it2 = bears.begin(); it2 != bears.end(); ++it2) {
            atacking[it2->index] |= CellFlags::CHARACTER;
        }
    } else if (state.type == CharacterType::BEAR) {
        for (auto it2 = humans.begin(); it2 != humans.end(); ++it2) {
            if (it2->player->state.alive) {
                atacking[it2->index] |= CellFlags::CHARACTER;
                suggest[it2->index] |= CellFlags::CHARACTER;
                for (auto ind : it2->trace) {
                    suggest[ind] |= CellFlags::TRACE;
                    atacking[ind] |= CellFlags::TRACE;
                }
            }
        }
    } else {
        throw std::runtime_error("error type");
    }
    _process_point(index, state);

    JSONObject info;
    info.add_items({{"alive", std::to_string(state.alive)},
                    {"treasure", std::to_string(state.treasure)},
                    {"ammos", std::to_string(state.ammos)},
                    {"bombs", std::to_string(state.bombs)},
                    {"type", std::to_string(state.type)},
                    {"name", state.name}});
    logger->log(info.to_string());

    // uint32_t answer = 0;
    uint32_t radius = (state.type == CharacterType::HUMAN)
                          ? settings.HUMAN_FOV_RADIUS
                          : settings.BEAR_FOV_RADIUS;

    uint32_t command = 0;
    try {
        command = player.suggest(suggest.neighborhood(index, radius));
    } catch (std::runtime_error &e) {
        player.inform(Answer::ERR | _end_turn(index, trace, state));
        logger->log(std::string("std::runtime_error(") + '"' + e.what() + '"' +
                    ')');
        return false;
    }

    bool success;
    Field::Index temp;

    switch (command & Command::COMMAND_MASK) {
    case Command::PASS:
        player.inform(Answer::NOTHING | _end_turn(index, trace, state));
        break;
    case Command::GO:
        // bool success;
        index = field.go_point(index, command, success);
        if (success) {
            player.inform(Answer::SUCCESS | _end_turn(index, trace, state));
        } else {
            player.inform(Answer::NOTHING | _end_turn(index, trace, state));
        }
        break;
    case Command::TRY_EXIT:
        if (field[index] & CellFlags::EXIT) {
            // we checked other conditions in Player.cpp
            player.inform(Answer::SUCCESS | _end_turn(index, trace, state));
            _winner = it->player;
            running = false;
        } else {
            player.inform(Answer::ERR | _end_turn(index, trace, state));
        }
        break;
    case Command::SHOOT:
        // bool success;
        temp = atacking.shoot_point(index, command, success);
        if (success) {
            std::transform(humans.begin(), humans.end(), humans.begin(),
                           [&field = field, temp,
                            ptr = it->player](Game::PlayerFullState full) {
                               if (full.index == temp && full.player != ptr) {
                                   auto &state = full.player->state;
                                   state.alive = false;
                                   if (state.treasure) {
                                       field[temp] |= CellFlags::TREASURE;
                                       state.treasure = false;
                                   }
                                   return full;
                               } else {
                                   return full;
                               }
                           });
            bears.erase(std::remove_if(bears.begin(), bears.end(),
                                       [temp](Game::PlayerFullState full) {
                                           return full.index == temp;
                                       }),
                        bears.end());
            player.inform(Answer::SUCCESS | _end_turn(index, trace, state));
        } else {
            player.inform(Answer::NOTHING | _end_turn(index, trace, state));
        }
        --state.ammos;
        break;
    case Command::EXPLODE:
        // bool success;
        field.explode(index, command, success);
        if (success) {
            player.inform(Answer::SUCCESS | _end_turn(index, trace, state));
        } else {
            player.inform(Answer::NOTHING | _end_turn(index, trace, state));
        }
        --state.bombs;
        break;
    case Command::BEAR_ATACK:
        if (atacking[index] & CellFlags::CHARACTER) {
            std::transform(
                humans.begin(), humans.end(), humans.begin(),
                [&field = field, index](Game::PlayerFullState full) {
                    if (full.index == index) {
                        auto &state = full.player->state;
                        state.alive = false;
                        if (state.treasure) {
                            field[index] |= CellFlags::TREASURE;
                            state.treasure = false;
                        }
                        return full;
                    } else {
                        return full;
                    }
                });
            player.inform(Answer::SUCCESS | _end_turn(index, trace, state));
        } else {
            player.inform(Answer::NOTHING | _end_turn(index, trace, state));
        }
        break;
    case Command::FINNISH_GAME:
        running = false;
        player.inform(Answer::SUCCESS | _end_turn(index, trace, state));
        break;
    case Command::GIVE_UP:
        player.inform(Answer::SUCCESS);
        if (state.treasure) {
            field[index] |= CellFlags::TREASURE;
        }
        if (state.type == HUMAN) {
            // humans.erase(it);
            return true;
        } else {
            // bears.erase(it);
            return true;
        }
        break;
    }
    return false;
}

void Game::step() {
    if (!is_running()) {
        throw std::runtime_error("!is_running()");
    }
    for (size_t ind = 0; ind < humans.size();) {
        auto it = humans.begin() + ind;
        auto to_remove = _one_turn(it);
        if (!is_running()) {
            return;
        }
        if (to_remove) {
            humans.erase(it);
            if (humans.size() == 0) {
                running = false;
                return;
            }
        } else {
            ++ind;
        }
    }
    for (size_t ind = 0; ind < bears.size();) {
        auto it = bears.begin() + ind;
        auto to_remove = _one_turn(it);
        if (!is_running()) {
            return;
        }
        if (to_remove) {
            bears.erase(it);
        } else {
            ++ind;
        }
    }
}

const Field &Game::get_field() const { return field; }

bool Game::is_running() const { return running; }

std::shared_ptr<Character> Game::winner() {
    if (is_running()) {
        throw std::runtime_error("the game still running");
    }
    if (_winner) {
        return _winner;
    }
    throw std::runtime_error("there is now winner somehow");
}
