#include "JSON.h"
#include "Player.h"
#include <iostream>

int main() {
    auto in = []() {
        std::string command;
        std::cin >> command;
        std::string direction;
        std::cin >> direction;
        JSONObject o;
        o.add_items({{"COMMAND", command}, {"DIRECTION", direction}});
        return o.to_string();
    };
    auto out = [](std::string s) { std::cout << s; };
    Player p(in, out);
    p.suggest({{2 + 4 + 8 + 256 + 1024}});
    return 0;
}
