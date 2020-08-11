#include "JSONChildren.h"

JSONString::JSONString(const std::string &s) : st(s) {}

std::string JSONString::to_string() const {
    return ([](std::string s) { return '"' + s + '"'; })(st);
}

const JSON &JSONString::operator[](std::string) const { return *this; }

std::string JSONString::type() const { return "JSONString"; }

std::vector<std::string> JSONString::keys() const {
    std::vector<std::string> res;
    return res;
}

JSONString::~JSONString() {}

JSONList::JSONList() {}

std::string JSONList::to_string() const {
    std::string st;
    st += '[';
    uint32_t i = 0;
    for (auto &item : items) {
        if (i) {
            st += ", ";
        }
        ++i;
        st += item->to_string();
    }
    st += ']';
    return st;
}

void JSONList::add_item(std::unique_ptr<JSON> j) {
    items.push_back(std::move(j));
}

const JSON &JSONList::operator[](std::string key) const {
    uint32_t i = std::stoi(key);
    assert(i < items.size());
    return *(items[i]);
}

std::string JSONList::type() const { return "JSONList"; }

std::vector<std::string> JSONList::keys() const {
    std::vector<std::string> result;
    for (uint32_t i = 0; i < items.size(); ++i) {
        result.push_back(std::to_string(i));
    }
    return result;
}

JSONList::~JSONList() {}

JSONObject::JSONObject() {}

std::string JSONObject::to_string() const {
    std::string st;
    st += '{';
    uint32_t i = 0;
    auto wrapper = [](std::string s) { return '"' + s + '"'; };
    for (auto &item : items) {
        if (i) {
            st += ", ";
        }
        ++i;
        st += wrapper(item.first) + " : " + item.second->to_string();
    }
    st += '}';
    return st;
}

const JSON &JSONObject::operator[](std::string key) const {
    assert(items.find(key) != items.end());
    return *(items.find(key)->second);
}

void JSONObject::add_item(std::string key, std::unique_ptr<JSON> value) {
    items.emplace(key, std::move(value));
}

std::string JSONObject::type() const { return "JSONObject"; }

std::vector<std::string> JSONObject::keys() const {
    std::vector<std::string> result;
    for (auto &item : items) {
        result.push_back(item.first);
    }
    return result;
}

JSONObject::~JSONObject() {}
