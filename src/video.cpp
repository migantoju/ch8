//
// Created by Miguel Toledano on 09/10/22.
//
#include <cstdio>

#include "video.h"

Graphics::Graphics(const char *title) {
    // Return 0 on success or a negative error code on failure
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("ERROR!, Unable to init SDL. SDL_Error: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow(
            title,                      // Window title
            SDL_WINDOWPOS_UNDEFINED, // Initial x position
            SDL_WINDOWPOS_UNDEFINED, // Initial y position
            640,                     // Width, in pixels
            480,                     // Height, in pixels
            SDL_WINDOW_SHOWN      // flags
            );

    // Check for window was successfully created.
    if(window == nullptr){
        printf("ERROR!, Unable to create window. SDL_Error: %s\n", SDL_GetError());
    }

    // Create 2D renderer context
    // Return a valid rendering context or NULL
    renderer = SDL_CreateRenderer(
            window,                           // Where rendering is displayed
            -1,                         // Index of rendering driver to init.
            SDL_RENDERER_ACCELERATED    // flags
            );


    if(renderer == nullptr) {
        printf("ERROR!, Unable to create renderer. SDL_Error: %s\n", SDL_GetError());
    }

    // Create texture for rendering context
    texture = SDL_CreateTexture(
            renderer,                              // Rendering context
            SDL_PIXELFORMAT_ABGR8888,       // Pixel Format
            SDL_TEXTUREACCESS_STREAMING,    // Texture access
            64,                                // Width of texture in pixels
            32                                 // Height of texture in pixels
            );

    if(texture == nullptr) {
        printf("ERROR!, Unable to create texture. SDL_Error: %s\n", SDL_GetError());
    }
}

Graphics::~Graphics() {
    // Destroy texture
    SDL_DestroyTexture(texture);
    texture = nullptr;
    // Destroy Renderer
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    // Destroy Window
    SDL_DestroyWindow(window);
    window = nullptr;

    // Clean up
    SDL_Quit();
}


