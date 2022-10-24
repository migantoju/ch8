//
// Created by Miguel Toledano on 09/10/22.
// Docs for SDL2: https://wiki.libsdl.org/APIByCategory
//
#include <SDL.h>

#ifndef CHIP8_VIDEO_H
#define CHIP8_VIDEO_H

#define WIDTH 1024
#define HEIGHT 512

class Graphics {
public:
    Graphics(const char *title);
    ~Graphics();

    void update(uint32_t (&pixels)[2048]);

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

};

#endif //CHIP8_VIDEO_H
