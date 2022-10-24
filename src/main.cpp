#include <iostream>
#include <cstdio>
#include <SDL.h>
#include <thread>
#include <chrono>

#include "cpu.h"
#include "video.h"

// Keypad map
uint8_t Keymap[16] = {
        SDLK_x,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_z,
        SDLK_c,
        SDLK_4,
        SDLK_r,
        SDLK_f,
        SDLK_v,
};

void Run(const char* filename) {
    CPU ch8;
    Graphics gfx("CHIP-8 Emulator");

    bool quit = false;

    uint32_t pixels[64 * 32];

    if(!ch8.loadRom(filename)) {
        std::cerr << "Error loading ROM" << std::endl;
    }

    while(!quit) {
        ch8.Cycle();

        // SDL Events
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) quit = true;

            if(e.type == SDL_KEYDOWN) {
                if(e.key.keysym.sym == SDLK_ESCAPE) quit = true;

                for(int i = 0; i < KEYPAD_SIZE; i++) {
                    if(e.key.keysym.sym == Keymap[i]) {
                        ch8.Keyboard[i] = 1;
                    }
                }
            }

            // keyup events
            if(e.type == SDL_KEYUP) {
                for(int i = 0; i < KEYPAD_SIZE; i++) {
                    if(e.key.keysym.sym == Keymap[i]) {
                        ch8.Keyboard[i] = 0;
                    }
                }
            }
        }

        if(ch8.shouldDraw) {
            ch8.shouldDraw = false;

            // Draw
            for(int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; i++) {
                uint8_t pixel = ch8.Display[i];
                pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
            }

            gfx.update(pixels);
        }

        // Sleep
        std::this_thread::sleep_for(std::chrono::microseconds(1200));
    }
}

int main(int argc, char* args[]) {
    try {
        if(argc != 2) {
            std::cerr << "Usage: chip8 <ROM file> " << std::endl;
            return 1;
        }
        Run(args[1]);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what();
        return 1;
    }
    return 0;
}

