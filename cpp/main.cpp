#include "Server.h"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cout << "usage: ./main PORT WORKERS TIMEOUT";
        return 0;
    }
    uint32_t port = std::stoi(argv[1]);
    uint32_t n_workers = std::stoi(argv[2]);
    uint32_t timeout = std::stoi(argv[3]);

    std::shared_ptr<Logger> log(new Logger(std::cout));
    Server s(log);
    s.Start(port, n_workers, timeout);
    std::cin.get();
    // std::cout << "error" << std::endl;
    s.Stop(true);
    return 0;
}
