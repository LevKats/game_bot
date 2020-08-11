#include "Player.h"
#include "JSON.h"

Player::Player(std::function<std::string()> in,
               std::function<void(std::string)> out)
    : Character(), in(in), out(out) {}

Player::~Player() {}

uint32_t Player::suggest(std::vector<std::vector<uint32_t>> cells) {
    JSONObject context;

    {
        std::unique_ptr<JSONObject> state(new JSONObject);

        {
            std::string typ;
            switch (get_state().type) {
            case CharacterType::BEAR:
                typ = "BEAR";
                break;
            case CharacterType::HUMAN:
                typ = "HUMAN";
                break;
            }

            state->add_items({
                {"ALIVE", std::to_string(get_state().alive)},
                {"AMMOS", std::to_string(get_state().ammos)},
                {"BOMBS", std::to_string(get_state().bombs)},
                {"TYPE", typ},
                {"NAME", get_state().name},
            });
        }

        context.add_item("STATE", std::move(state));
    }

    {
        std::unique_ptr<JSONList> cells_list(new JSONList);

        for (auto &row : cells) {
            std::unique_ptr<JSONList> row_list(new JSONList);

            for (auto cell : row) {
                std::unique_ptr<JSONObject> cell_object(new JSONObject);
                auto wrap = [](uint32_t a) { return std::to_string(bool(a)); };
                cell_object->add_items({
                    {"HIDDEN", wrap(CellFlags::HIDDEN & cell)},

                    {"CHARACTER", wrap(CellFlags::CHARACTER & cell)},
                    {"TRACE", wrap(CellFlags::TRACE & cell)},

                    {"LEFT_BORDER", wrap(CellFlags::LEFT_BORDER & cell)},
                    {"RIGHT_BORDER", wrap(CellFlags::RIGHT_BORDER & cell)},
                    {"UP_BORDER", wrap(CellFlags::UP_BORDER & cell)},
                    {"DOWN_BORDER", wrap(CellFlags::DOWN_BORDER & cell)},

                    {"HOSPITAL", wrap(CellFlags::HOSPITAL & cell)},
                    {"ARMORY", wrap(CellFlags::ARMORY & cell)},

                    {"WORM_HOLL", wrap(CellFlags::WORM_HOLL & cell)},

                    {"EXIT", wrap(CellFlags::EXIT & cell)},
                    {"TREASURE", wrap(CellFlags::TREASURE & cell)},
                });

                row_list->add_item(std::move(cell_object));
            }

            cells_list->add_item(std::move(row_list));
        }

        context.add_item("CELLS", std::move(cells_list));
    }

    out(context.pretty_print());

    auto ans = JSONParser(in()).parse();

    std::string command_name;
    std::string direction;
    try {
        command_name = (*ans)["COMMAND"].text();
        direction = (*ans)["DIRECTION"].text();
    } catch (std::runtime_error &e) {
        throw std::runtime_error("Incorrect command JSON format");
    }

    uint32_t res = 0;

    if (command_name == "PASS") {
        res |= Command::PASS;
    } else if (command_name == "GO") {
        res |= Command::GO;
    } else if (command_name == "TRY_EXIT") {
        res |= Command::TRY_EXIT;
    } else if (command_name == "SHOOT") {
        res |= Command::SHOOT;
    } else if (command_name == "EXPLODE") {
        res |= Command::EXPLODE;
    } else if (command_name == "BEAR_ATACK") {
        res |= Command::BEAR_ATACK;
    } else {
        throw std::runtime_error("Invalid command");
    }

    if (direction == "LEFT") {
        res |= Command::LEFT;
    } else if (direction == "RIGHT") {
        res |= Command::RIGHT;
    } else if (direction == "UP") {
        res |= Command::UP;
    } else if (direction == "DOWN") {
        res |= Command::DOWN;
    } else if (direction == "NONE") {
        res |= Command::NONE;
    } else {
        throw std::runtime_error("Invalid command");
    }

    return res;
}

void Player::inform(uint32_t ans) {
    std::string status;
    switch (ans & Answer::STATUS_MASK) {
    case Answer::SUCCESS:
        status = "SUCCESS";
        break;
    case Answer::NOTHING:
        status = "NOTHING";
        break;
    case Answer::UNK:
        status = "UNK";
        break;
    case Answer::ERR:
        status = "ERR";
        break;
    }
    std::string holl = std::to_string(bool(ans & Answer::HOLL_MASK));

    JSONObject res;
    res.add_items({{"STATUS", status}, {"HOLL", holl}});

    out(res.pretty_print());
}
