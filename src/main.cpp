#include <iostream>
#include "cpu.h"

void Run() {
    CPU ch8;

    std::string filename = "pong.ch8";

    if(!ch8.loadRom(filename)) {
        std::cerr << "Error loading ROM" << std::endl;
    }

    std::cout << "OK" << std::endl;
}

int main(int argc, char* args[]) {
    try {
        Run();
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what();
        return 1;
    }
    return 0;
}

