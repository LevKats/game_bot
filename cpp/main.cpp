#include "Game.h"
#include "JSON.h"
#include <iostream>

int main() {
    Logger l(std::cout);
    std::shared_ptr<Player> human(new Player(
        []() {
            std::string command;
            std::cin >> command;
            std::string direction;
            std::cin >> direction;
            JSONObject res;
            res.add_items({{"COMMAND", command}, {"DIRECTION", direction}});
            return res.to_string();
        },
        [](std::string s) { std::cout << s; }, "lev"));
    std::shared_ptr<Player> bear(new Player(
        []() {
            // std::string command;
            // std::cin >> command;
            // std::string direction;
            // std::cin >> direction;
            JSONObject res;
            // res.add_items({{"COMMAND", command}, {"DIRECTION", direction}});
            res.add_items({{"COMMAND", "PASS"}, {"DIRECTION", "NONE"}});
            return res.to_string();
        },
        [](std::string s) { std::cout << s; }, "bear"));
    Field f(10, 3);
    auto player = Game::PlayerFullState{human, Field::Index{3, 4},
                                        std::vector<Field::Index>()};
    Game g({{bear, {1, 2}, {}}}, {player}, f, l, {3, 1, 2, 3, 3});
    while (g.is_running()) {
        g.step();
    }
    return 0;
}
