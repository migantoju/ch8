//
// Created by Miguel Toledano on 09/10/22.
// Docs for SDL2: https://wiki.libsdl.org/APIByCategory
//
#include <SDL.h>

#ifndef CHIP8_VIDEO_H
#define CHIP8_VIDEO_H

#endif //CHIP8_VIDEO_H

class Graphics {
public:
    Graphics(const char *title);
    ~Graphics();

    void Update(void const* display, int pitch);

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

};