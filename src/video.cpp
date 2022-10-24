//
// Created by Miguel Toledano on 09/10/22.
//
#include <cstdio>

#include "video.h"

Graphics::Graphics(const char *title) {
    // Init SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        exit(1);
    }

    // create window
    window = SDL_CreateWindow(
            title,                          // Window title
            SDL_WINDOWPOS_UNDEFINED,     // Initial window X position
            SDL_WINDOWPOS_UNDEFINED,     // Initial window Y position
            WIDTH,                      // Width in pixels
            HEIGHT,                     // Height in pixels
            SDL_WINDOW_SHOWN         // flags
            );

    if(window == nullptr) {
        printf("ERROR! Unable to create window. SDL Error %s\n", SDL_GetError());
        exit(2);
    }

    // create renderer
    renderer = SDL_CreateRenderer(
            window,                             // Where rendering is displayed
            -1,                           // Index of rendering driver to init
            SDL_RENDERER_ACCELERATED      // flags
            );
    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

    if (renderer == nullptr) {
        printf("ERROR! unable to create renderer. SDL Error: %s\n", SDL_GetError());
        exit(3);
    }

    // Created texture
    texture = SDL_CreateTexture(
            renderer,   // Rendering context
            SDL_PIXELFORMAT_ARGB8888, // Pixel format
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC, // Texture access
            64, // Width of texture in pixels
            32  // Height of texture in pixels
            );

    if(texture == nullptr) {
        printf("ERROR! Unable to create texture. SDL Error: %s\n", SDL_GetError());\
        exit(4);
    }
}

void Graphics::update(uint32_t (&pixels)[2048]) {
    // update texture
    SDL_UpdateTexture(texture, nullptr, &pixels, 64 * sizeof(uint32_t));

    // Clear screen and render
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

Graphics::~Graphics() {
    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyTexture(texture);
    texture = nullptr;
}
