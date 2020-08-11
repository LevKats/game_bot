#pragma once
#include "JSONBase.h"

class JSONString : public JSON {
public:
    JSONString(const std::string &);

    std::string to_string() const override;

    const JSON &operator[](std::string) const override;

    std::string type() const override;

    std::vector<std::string> keys() const override;

    ~JSONString();

private:
    std::string st;
};

class JSONList : public JSON {
public:
    JSONList();

    std::string to_string() const override;

    void add_item(std::unique_ptr<JSON>);

    const JSON &operator[](std::string key) const override;

    std::string type() const override;

    std::vector<std::string> keys() const override;

    ~JSONList();

private:
    std::vector<std::unique_ptr<JSON>> items;
};

class JSONObject : public JSON {
public:
    JSONObject();

    std::string to_string() const override;

    const JSON &operator[](std::string key) const override;

    void add_item(std::string key, std::unique_ptr<JSON> value);

    std::string type() const override;

    std::vector<std::string> keys() const override;

    ~JSONObject();

private:
    std::map<std::string, std::unique_ptr<JSON>> items;
};
