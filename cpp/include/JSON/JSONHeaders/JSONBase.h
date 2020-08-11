#pragma once
#include <cassert>
#include <exception>
#include <map>
#include <memory>
#include <string>
#include <vector>

class JSON {
public:
    JSON();

    virtual ~JSON();

    virtual std::string to_string() const = 0;

    virtual std::string type() const = 0;

    virtual std::vector<std::string> keys() const = 0;

    virtual const JSON &operator[](std::string) const = 0;

    std::string pretty_print(std::string tab = std::string("")) const;

    std::string text() const;
};
