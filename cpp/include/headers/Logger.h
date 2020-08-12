#pragma once
#include <mutex>
#include <ostream>
#include <thread>

class Logger {
public:
    Logger(std::ostream &out);

    ~Logger();

    void log(std::string);

private:
    std::mutex m;
    std::ostream &out;
};
