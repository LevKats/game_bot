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

    virtual std::string to_string() const;

    virtual std::string type() const;

    virtual std::vector<std::string> keys() const;

    virtual const JSON &operator[](std::string) const;

    std::string pretty_print(std::string tab = std::string("")) const;
};
