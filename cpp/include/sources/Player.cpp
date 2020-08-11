#include "Player.h"
#include "JSON.h"

Player::Player(std::istream &in, std::ostream &out)
    : Character(), in(in), out(out) {}

Player::~Player() {}

uint32_t Player::suggest(std::vector<std::vector> cells) {
    JSONObject context;

    {
        std::unique_ptr<JSON> state(new JSONObject);

        {
            state->add_item("alive", std::unique_ptr<JSON>(new JSONString(
                                         std::to_string(get_state().alive))));
            state->add_item("ammos", std::unique_ptr<JSON>(new JSONString(
                                         std::to_string(get_state().ammos))));
            state->add_item("bombs", std::unique_ptr<JSON>(new JSONString(
                                         std::to_string(get_state().bombs))));
            state->add_item("type", std::unique_ptr<JSON>(new JSONString(
                                        std::to_string(get_state().type))));
            state->add_item("name", std::unique_ptr<JSON>(new JSONString(
                                        std::to_string(get_state().name))));
        }

        context.add_item("state", std::move(state));
    }

    {
        std::unique_ptr<JSON> cells_list(new JSONList);

        for (auto &row : cells) {
            std::unique_ptr<JSON> row_list(new JSONList);

            for (auto cell : row) {
                row_list->add_item(std::unique_ptr<JSON>(
                    new JSONString(std::to_string(cell))));
            }

            cells_list->add_item(std::move(row_list));
        }

        context.add_item("cells", std::move(cells_list));
    }

    out << context.pretty_print();

    uint32_t res;
    in >> res;

    return res;
}

void Player::inform(Answer ans) { out }
