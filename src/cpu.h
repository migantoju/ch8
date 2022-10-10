//
// Created by Miguel Toledano on 09/10/22.
//
#include <array>

#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

#endif //CHIP8_CPU_H

const int MEMORY_SIZE = 4096;
const int REGISTERS_SIZE = 16;
const int STACK_SIZE = 16;
const int KEYPAD_SIZE = 16;
const short int VIDEO_WIDTH = 64;
const short int VIDEO_HEIGHT = 32;

class CPU {
private:
    std::array<uint8_t, MEMORY_SIZE> Memory{}; // RAM 4KB
    std::array<uint8_t, REGISTERS_SIZE> V{}; // Registers Vx - VF
    std::array<uint16_t, STACK_SIZE> S{}; // Stack
    std::array<uint8_t, KEYPAD_SIZE> Keyboard{}; // Keypad Hex
    std::array<uint16_t, VIDEO_WIDTH * VIDEO_HEIGHT> Display{}; // Display
    std::array<uint16_t, STACK_SIZE> I{}; // Index

    uint16_t opcode{}; // Current Opcode
    uint8_t DT{}; // Delay TImer
    uint8_t ST{}; // Sound Timer
    uint8_t SP{}; // Stack Pointer
    uint16_t PC{}; // Program Counter
public:
    CPU();
    // ~CPU();

    void Reset();
    bool loadRom(std::string const& filename);
    void Cycle();
    void FetchOpcode();

    bool shouldDraw = false;
    bool playSound = false;

    // here goes Function Pointer Table.
};