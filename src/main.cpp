#include <iostream>
#include <SDL.h>

#include "cpu.h"

int main(int argc, char* args[]) {
    std::string filename = "pong.ch8";

    CPU ch8;
    ch8.loadRom(filename);

    return 0;
}
