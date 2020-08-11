#include "JSONBase.h"

JSON::JSON() {}

JSON::~JSON() {}

std::string JSON::to_string() const { return ""; }

std::string JSON::type() const { return "JSON base"; }

std::vector<std::string> JSON::keys() const {
    return std::vector<std::string>();
}

const JSON &JSON::operator[](std::string) const { return (*this); }

std::string JSON::pretty_print(std::string tab) const {
    if (type() == "JSONString") {
        return tab + to_string();
    } else if (type() == "JSONList") {
        std::string result = tab + '[' + '\n';
        auto indices = keys();
        uint32_t i = 0;
        for (auto k : indices) {
            result += (*this)[k].pretty_print(tab + "    ");
            if (i != indices.size() - 1) {
                result += ",";
            }
            ++i;
            result += '\n';
        }
        result += tab + ']';
        return result;
    } else if (type() == "JSONObject") {
        std::string result = tab + '{' + '\n';
        auto indices = keys();
        uint32_t i = 0;
        auto wrapper = [](std::string s) { return '"' + s + '"'; };
        for (auto k : indices) {
            result += tab + "    " + wrapper(k) + " :" + '\n';
            result += (*this)[k].pretty_print(tab + "        ");
            if (i != indices.size() - 1) {
                result += ",";
            }
            ++i;
            result += '\n';
        }
        result += tab + '}';
        return result;
    } else {
        throw std::runtime_error("Incorrect type");
    }
}
