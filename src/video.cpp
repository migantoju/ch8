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
            1280,                     // Width, in pixels
            720,                     // Height, in pixels
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
            SDL_PIXELFORMAT_RGBA8888,       // Pixel Format
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

void Graphics::Update(int pitch, const uint8_t display[64 * 32]) {
    // test 1
    uint32_t pixels[64 * 32];

    for(int i = 0; i < 32 * 64; i++) {
        if(display[i] == 0) {
            pixels[i] = 0xFF000000;
        } else {
            pixels[i] = 0xFFFFFFFF;
        }
    }

    // Update the given texture
    auto updateTexture = SDL_UpdateTexture(
            texture,        // Texture to update
            nullptr,    // SDL_Rect structure or NULL to update entire texture
            pixels,    // Raw pixel data
            pitch             // Number of bytes in a row of pixel data
    );

    if(updateTexture < 0) {
        printf("ERROR!, Unable to update texture. SDL_Error: %s\n", SDL_GetError());
    }

    // Clears the entire rendering target
    SDL_RenderClear(renderer);

    // Copy a portion of the texture to the current rendering target
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    // Update the screen with any rendering performed since the previous call.
    SDL_RenderPresent(renderer);

}


