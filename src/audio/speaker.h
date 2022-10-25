//
// Created by Miguel Toledano on 23/10/2022.
//

#ifndef CHIP8_SPEAKER_H
#define CHIP8_SPEAKER_H

#include <SDL.h>
#include <array>

class Speaker {
    public:
        ~Speaker();

        bool init();

        void Beep(int length);

    private:
        static constexpr double kFrecuency = 48000;
        static constexpr double kTone = 440;
        static constexpr int kAmplitude = 7;
        static constexpr int kBias = 127;
        static constexpr int kSamplePerFrame = (kFrecuency / 60) * 3;

        std::array<unsigned char, kSamplePerFrame * 30> audioBuffer{};

        double wave_position = 0;
        double wave_increment = (kTone * (2.0 * M_PI) / kFrecuency);

        SDL_AudioDeviceID device{};

        void SineWave(int length);

    };

#endif //CHIP8_SPEAKER_H
