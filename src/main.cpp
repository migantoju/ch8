#include <iostream>
#include <cstdio>
#include <SDL.h>
#include <thread>
#include <chrono>

#include "cpu.h"

// Keypad map
uint8_t Keymap[16] = {
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_4,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_r,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_f,
        SDLK_z,
        SDLK_x,
        SDLK_c,
        SDLK_v,
};

void Run() {
    CPU ch8;

    std::string filename = "test_opcode.ch8";

    // SDL Video
    int w = 1024;
    int h = 512;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL Could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
    if(window == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(2);
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, w, h);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    // pixel buffer
    uint32_t pixels[VIDEO_WIDTH * VIDEO_HEIGHT];

    bool quit = false;

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

            // store pixels in temp buffer
            for (int i = 0; i < VIDEO_WIDTH*VIDEO_HEIGHT; ++i) {
                uint8_t pixel = ch8.Display[i];
                pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
            }

            // update SDL Texture
            SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t));
            // Clear screen and reder
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }

        // Sleep
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
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

