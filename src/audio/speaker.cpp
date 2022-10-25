//
// Created by Miguel Toledano on 23/10/2022.
//
#include "speaker.h"

#include <iostream>
#include <cmath>

bool Speaker::init() {
    SDL_AudioSpec spec;
    spec.freq = kFrecuency;
    spec.format = AUDIO_U8;
    spec.channels = 1;
    spec.samples = 2048;
    spec.callback = nullptr;
    spec.userdata = nullptr;

    device = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);


    if(!device) {
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_PauseAudioDevice(device, 0);

    return true;
}

void Speaker::Beep(int length) {
    if(SDL_GetQueuedAudioSize(device) < (kSamplePerFrame * 2)) {
        SineWave(length);
        SDL_QueueAudio(device, audioBuffer.data(), length);
    }
}

void Speaker::SineWave(int length) {
    for(int i = 0; i < length; i++) {
        audioBuffer[i] = (unsigned char) ((kAmplitude * sin(wave_position)) * kBias);
        wave_position += wave_increment;
    }
}

Speaker::~Speaker() {
    SDL_PauseAudioDevice(device, 1);
    if(device) {
        SDL_CloseAudioDevice(device);
    }
}