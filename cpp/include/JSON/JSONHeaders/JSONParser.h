#pragma once
#include "JSONBase.h"
#include "JSONChildren.h"

class JSONParser {
public:
    JSONParser(std::string);

    ~JSONParser();

    std::unique_ptr<JSON> parse();

private:
    std::pair<std::string, std::unique_ptr<JSON>> parse_pair();

    std::string parse_string();

    std::unique_ptr<JSON> parse_list();

    std::unique_ptr<JSON> parse_obj();

    std::string st;
    std::unique_ptr<JSON> result;
    std::string::iterator ptr;
};
