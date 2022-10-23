//
// Created by Miguel Toledano on 09/10/22.
//
#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

#endif //CHIP8_CPU_H

const int MEMORY_SIZE = 4096;
const int REGISTERS_SIZE = 16;
const int STACK_SIZE = 16;
const int KEYPAD_SIZE = 16;
const short int VIDEO_WIDTH = 64;
const short int VIDEO_HEIGHT = 32;
const uint8_t FONTSET_SIZE = 80;
const int START_ADDRESS = 0x200;
const int MAX_FILE_SIZE = MEMORY_SIZE - START_ADDRESS;

class CPU {
private:
    uint8_t Memory[MEMORY_SIZE]{}; // Memory RAM 4Kb
    uint8_t V[REGISTERS_SIZE]{}; // Registers V0 - VF
    uint16_t S[STACK_SIZE]{}; // Stack

    uint16_t opcode{}; // Current Opcode
    uint8_t DT{}; // Delay TImer
    uint8_t ST{}; // Sound Timer
    uint16_t SP{}; // Stack Pointer
    uint16_t PC{}; // Program Counter
    uint16_t I{};   // Index

    void FetchOpcode();
    static int randomByte();

public:
    CPU();
    // ~CPU();

    uint8_t Keyboard[16]{};
    uint16_t Display[VIDEO_WIDTH * VIDEO_HEIGHT]{};

    bool loadRom(std::string const& filename);
    void Cycle();

    bool shouldDraw = false;
    bool playSound = false;

    // here goes Function Pointer Table.
};