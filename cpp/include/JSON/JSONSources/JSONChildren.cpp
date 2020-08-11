#include "JSONChildren.h"

JSONString::JSONString(const std::string &s) : st(s) {}

std::string JSONString::to_string() const {
    return ([](std::string s) { return '"' + s + '"'; })(st);
}

const JSON &JSONString::operator[](std::string) const {
    throw std::runtime_error("no [] supported");
}

std::string JSONString::type() const { return "JSONString"; }

std::vector<std::string> JSONString::keys() const {
    throw std::runtime_error("no keys supported");
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

void JSONList::add_item(const std::string &s) {
    add_item(std::unique_ptr<JSON>(new JSONString(s)));
}

void JSONList::add_items(const std::vector<std::string> &v) {
    for (auto &str : v) {
        add_item(str);
    }
}

const JSON &JSONList::operator[](std::string key) const {
    uint32_t i;
    try {
        i = std::stoi(key);
    } catch (std::invalid_argument &e) {
        throw std::runtime_error("unsupported_index");
    } catch (std::out_of_range &e) {
        throw std::runtime_error("unsupported_index");
    }
    if (i >= items.size()) {
        throw std::runtime_error("unsupported_index");
    }
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
    if (items.find(key) == items.end()) {
        throw std::runtime_error("unsupported_index");
    }
    return *(items.find(key)->second);
}

void JSONObject::add_item(std::string key, std::unique_ptr<JSON> value) {
    items.emplace(key, std::move(value));
}

void JSONObject::add_item(std::string key, const std::string &value) {
    add_item(key, std::unique_ptr<JSON>(new JSONString(value)));
}

void JSONObject::add_items(const std::map<std::string, std::string> &m) {
    for (auto &p : m) {
        add_item(p.first, p.second);
    }
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
