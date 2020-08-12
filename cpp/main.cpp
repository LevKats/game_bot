#include "Logger.h"
#include <iostream>

int main() {
    Logger l(std::cout);
    l.log("some important information");
    return 0;
}
