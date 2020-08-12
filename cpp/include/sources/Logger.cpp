#include "Logger.h"
#include <chrono>
#include <iomanip>

Logger::Logger(std::ostream &out) : out(out) {}

Logger::~Logger() {}

void Logger::log(std::string s) {
    {
        std::unique_lock<std::mutex> lock(m);
        std::chrono::system_clock::time_point now =
            std::chrono::system_clock::now();
        std::time_t t_c = std::chrono::system_clock::to_time_t(now);
        out << std::put_time(std::localtime(&t_c), "%F %T") << " | ";
        out << "thread id ";
        out << std::setw(7) << std::this_thread::get_id();
        out << " | " << s << std::endl;
    }
}
