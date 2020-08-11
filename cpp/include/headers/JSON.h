#pragma once
#include "JSONBase.h"
#include "JSONChildren.h"
#include "JSONParser.h"

/*
#pragma once
#include <cassert>
#include <exception>
#include <map>
#include <memory>
#include <string>
#include <vector>

class JSON {
public:
    JSON() {}

    virtual ~JSON() {}

    virtual std::string to_string() const { return ""; }

    virtual std::string type() const { return "JSON base"; }

    virtual std::vector<std::string> keys() const {
        return std::vector<std::string>();
    }

    virtual const JSON &operator[](std::string) const { return (*this); }

    std::string pretty_print(std::string tab = std::string("")) const {
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
};

class JSONString : public JSON {
public:
    JSONString(const std::string &s) : st(s) {}

    std::string to_string() const override {
        return ([](std::string s) { return '"' + s + '"'; })(st);
    }

    const JSON &operator[](std::string) const override { return *this; }

    std::string type() const override { return "JSONString"; }

    std::vector<std::string> keys() const override {
        std::vector<std::string> res;
        return res;
    }

    ~JSONString() {}

private:
    std::string st;
};

class JSONList : public JSON {
public:
    JSONList() {}

    std::string to_string() const override {
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

    void add_item(std::unique_ptr<JSON> j) { items.push_back(std::move(j)); }

    const JSON &operator[](std::string key) const override {
        uint32_t i = std::stoi(key);
        assert(i < items.size());
        return *(items[i]);
    }

    std::string type() const override { return "JSONList"; }

    std::vector<std::string> keys() const override {
        std::vector<std::string> result;
        for (uint32_t i = 0; i < items.size(); ++i) {
            result.push_back(std::to_string(i));
        }
        return result;
    }

    ~JSONList() {}

private:
    std::vector<std::unique_ptr<JSON>> items;
};

class JSONObject : public JSON {
public:
    JSONObject() {}

    std::string to_string() const override {
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

    const JSON &operator[](std::string key) const override {
        assert(items.find(key) != items.end());
        return *(items.find(key)->second);
    }

    void add_item(std::string key, std::unique_ptr<JSON> value) {
        items.emplace(key, std::move(value));
    }

    std::string type() const override { return "JSONObject"; }

    std::vector<std::string> keys() const override {
        std::vector<std::string> result;
        for (auto &item : items) {
            result.push_back(item.first);
        }
        return result;
    }

    ~JSONObject() {}

private:
    std::map<std::string, std::unique_ptr<JSON>> items;
};

class JSONParser {
public:
    JSONParser(std::string st) : st(st), result(new JSON), ptr(st.begin()) {}

    ~JSONParser() {}

    std::unique_ptr<JSON> parse() {
        for (; ptr != st.end(); ++ptr) {
            if (*ptr == '{') {
                return parse_obj();
            } else if (*ptr == '[') {
                return parse_list();
            }
        }
        throw std::runtime_error("empty string or spaces");
    }

private:
    std::unique_ptr<JSON> parse_string() {}

    std::unique_ptr<JSON> parse_list() {}

    std::unique_ptr<JSON> parse_obj() {}

    std::string st;
    std::unique_ptr<JSON> result;
    std::string::iterator ptr;
};
*/
