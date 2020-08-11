#include "JSONParser.h"

JSONParser::JSONParser(std::string s) {
    st = "";
    bool is_string = false;
    for (auto c : s) {
        if (c == '"') {
            is_string = !is_string;
        }
        if (is_string) {
            st += c;
        } else {
            if (c != ' ' && c != '\n') {
                st += c;
            }
        }
    }
    ptr = st.begin();
}

JSONParser::~JSONParser() {}

std::unique_ptr<JSON> JSONParser::parse() {
    if (ptr == st.end()) {
        throw std::runtime_error("string empty or spaces");
    } else if (ptr[0] == '{') {
        return parse_obj();
    } else if (ptr[0] == '[') {
        return parse_list();
    } else if (ptr[0] == '"') {
        return std::unique_ptr<JSON>(new JSONString(parse_string()));
    } else {
        throw std::runtime_error("invalid symbols");
    }
}

std::pair<std::string, std::unique_ptr<JSON>> JSONParser::parse_pair() {
    auto key = parse_string();
    if (ptr == st.end() || *ptr != ':') {
        throw std::runtime_error("pair problem");
    } else {
        ++ptr;
        auto value = parse();
        return std::make_pair(key, std::move(value));
    }
}

std::string JSONParser::parse_string() {
    if (ptr == st.end()) {
        throw std::runtime_error("string problem");
    } else if (ptr[0] != '"') {
        throw std::runtime_error("invalid symbols");
    } else {
        ++ptr;
        std::string result = "";
        for (; ptr != st.end() && *ptr != '"'; ++ptr) {
            // std::cout << 's' << *ptr << "\n";
            result += *ptr;
        }
        if (ptr == st.end()) {
            throw std::runtime_error("string problem");
        } else {
            ++ptr;
            return result;
        }
    }
}

std::unique_ptr<JSON> JSONParser::parse_list() {
    if (ptr == st.end()) {
        throw std::runtime_error("list problem");
    } else if (ptr[0] != '[') {
        throw std::runtime_error("invalid symbols");
    }

    ++ptr;
    std::unique_ptr<JSONList> result(new JSONList);

    for (; ptr != st.end() && *ptr != ']'; ++ptr) {
        // std::cout << 'l' << *ptr << "\n";
        result->add_item(parse());
        if (*ptr != ',' && *ptr != ']') {
            throw std::runtime_error("list problem");
        } else if (*ptr == ']') {
            break;
        }
    }
    if (ptr == st.end()) {
        throw std::runtime_error("list problem");
    } else {
        ++ptr;
        return result;
    }
}

std::unique_ptr<JSON> JSONParser::parse_obj() {
    if (ptr == st.end()) {
        throw std::runtime_error("obj problem");
    } else if (ptr[0] != '{') {
        throw std::runtime_error("invalid symbols");
    }

    ++ptr;
    std::unique_ptr<JSONObject> result(new JSONObject);

    for (; ptr != st.end() && *ptr != '}'; ++ptr) {
        // std::cout << 'o' << *ptr << "\n";
        auto p = parse_pair();
        result->add_item(p.first, std::move(p.second));
        if (*ptr != ',' && *ptr != '}') {
            throw std::runtime_error("obj problem");
        } else if (*ptr == '}') {
            break;
        }
    }
    if (ptr == st.end()) {
        throw std::runtime_error("obj problem");
    } else {
        ++ptr;
        return result;
    }
}
