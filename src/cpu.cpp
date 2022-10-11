//
// Created by Miguel Toledano on 09/10/22.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "cpu.h"
#include "masks.h"


const uint8_t FONTSET_SIZE = 80;
const int START_ADDRESS = 0x200;

int rb;

std::array<uint8_t, FONTSET_SIZE> fontset {
        0xF0, 0x90 ,0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

CPU::CPU() {

    // Set PC to initial position
    PC = START_ADDRESS;
    I = 0x0000;
    opcode = 0;
    SP = 0x0;

    // Load Fonts
    std::copy(fontset.begin(), fontset.end(), Memory.begin() + 80);
}

bool CPU::loadRom(std::string const& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if(!file) {
        return false;
    }
    // Get size of file
    std::streampos size = file.tellg();

    if (size == std::ifstream::pos_type(-1)) {
        return false;
    }

    // char* buffer = new char[size];
    std::vector<uint8_t> buffer(size);

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    file.close();

    // if size > Memory size, won't fit in memory
    if (size > MEMORY_SIZE - 0x200) {
        return false;
    }

    std::copy(buffer.begin(), buffer.end(), Memory.begin() + 0x200);

    return true;
}

void CPU::Cycle() {
    FetchOpcode();

    switch (opcode & GET_HIGH_NIBBLE) {
        case 0x0000:
            switch (opcode & GET_4BITS) {
                case 0x00E0:
                    std::cout << "....Clearing Screen...." << std::endl;
                    std::cout << "CLS" << std::endl;

                    for(int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; i++){
                        Display[i] = 0x0;
                    }
                    shouldDraw = true;
                    break;
                case 0x00EE:
                    std::cout << "RET \nReturn from a subroutine" << std::endl;
                    PC = S[SP];
                    SP--;
                    break;
            }
        case 0x1000:
            // JUMP to location nnn
            std::cout << "JP addr: " << opcode << std::endl;
            PC = opcode & GET_ADDRESS;
            break;
        case 0x2000:
            std::cout << "CALL addr: " << opcode << std::endl;
            SP++;
            S[SP] = PC;
            PC = opcode & GET_ADDRESS;
            break;
        case 0x3000:
            // Skip the next instruction if Vx = kk
            std::cout << "SE Vx, byte" << std::endl;
            if(V[(opcode&GET_X) >> 8] == (opcode & GET_8BITS)) {
                PC += 2;
            }
            break;
        case 0x4000:
            // SKip the next instruction if Vx != kk
            std::cout << "SNE Vx, byte" << std::endl;
            if(V[(opcode & GET_X) >> 8] != (opcode&GET_8BITS)) {
                PC += 2;
            }
            break;
        case 0x5000:
            // Skip next instruction if Vx = Vy
            std::cout << "SE Vx, Vy" << std::endl;
            if(V[(opcode & GET_X) >> 8] == V[(opcode & GET_Y) >> 4]) {
                PC +=2;
            }
            break;
        case 0x6000:
            // Set Vx = kk
            std::cout << "LD Vx, byte" << std::endl;
            V[(opcode & GET_X) >> 8] = opcode & GET_8BITS;
            break;
        case 0x7000:
            // Set Vx = Vx + kk
            std::cout << "ADD Vx, byte" << std::endl;
            V[(opcode & GET_X) >> 8] += V[(opcode & GET_Y) >> 4];
            break;
        case 0x8000:
            switch (opcode & GET_4BITS) {
                case 0x0000:
                    // Set Vx = Vy
                    // Stores the value of register Vy in register Vx
                    V[(opcode&GET_X) >> 8] = V[(opcode&GET_Y) >> 4];
                    break;
                case 0x0001:
                    // Set Vx = Vx OR Vy
                    V[(opcode&GET_X) >> 8] = V[(opcode&GET_X) >> 8] | V[(opcode&GET_Y) >> 4];
                    break;
                case 0x0002:
                    // Set Vx = Vx AND Vy
                    V[(opcode&GET_X) >> 8] = V[(opcode&GET_X) >> 8] & V[(opcode&GET_Y) >> 4];
                    break;
                case 0x0003:
                    // Set Vx = Vx XOR Vy
                    V[(opcode&GET_X) >> 8] = V[(opcode&GET_X) >> 8] ^ V[(opcode&GET_Y) >> 4];
                    break;
                case 0x0004:
                    // Set Vx = Vx + Vy, set VF = carry
                    V[0xF] = (V[(opcode&GET_X) >> 8] + V[(opcode&GET_Y) >> 4]) > 0xFF ? 1:0;

                    V[(opcode&GET_X) >> 8] = (V[(opcode&GET_X) >> 8] + V[(opcode&GET_Y) >> 4] & GET_8BITS);
                    break;
                case 0x0005:
                    // Set Vx = Vx - Vy, set VF = NOT borrow.
                    V[0xF] = V[(opcode&GET_X) >> 8] > V[(opcode&GET_Y) >> 4] ? 1:0;
                    V[(opcode&GET_X) >> 8] = V[(opcode&GET_X) >> 8] - V[(opcode&GET_Y) >> 4];
                    break;
                case 0x0006:
                    // Set Vx = Vx SHR 1.
                    V[0xF] = 0;
                    if((V[(opcode&GET_X) >> 8] & 0x01) == 0x01) {
                        V[0xF] = 1;
                    }
                    V[(opcode&GET_X) >> 8] /= 2;
                    break;
                case 0x0007:
                    // Set Vx = Vy - Vx, srt VF = NOT borrow
                    V[0xF] = V[(opcode&GET_X) >> 8] > V[(opcode&GET_Y) >> 4] ? 1:0;
                    V[(opcode&GET_X) >> 8] = V[(opcode&GET_Y) >> 4] - V[(opcode&GET_X) >> 8];
                    break;
                case 0x000E:
                    // Set Vx = Vx SHL 1
                    V[0xF] = 0;
                    if((V[(opcode&GET_X) >> 8] & 0x10) == 0x10) {
                        V[0xF] = 1;
                    }
                    V[(opcode&GET_X) >> 8] *= 2;
                    break;
            }
        case 0x9000:
            // Skip next instruction if Vx != Vy
            if(V[(opcode&GET_X) >> 8] != V[(opcode&GET_Y) >> 4]) {
                PC += 2;
            }
            break;
        case 0xA000:
            // Set I = nnn
            I = opcode & GET_8BITS;
            break;
        case 0xB000:
            // Jump to location nnn + V0
            PC = (opcode&GET_ADDRESS) + V[0xF];
            break;
        case 0xC000:
            // Set Vx = random byte AND kk;
            rb = (rand() % 255) + 1;
            V[(opcode&GET_X) >> 8] = rb & (opcode&GET_8BITS);
            break;
        case 0xD000:
            std::cout << "DRW Vx, Vy, nibble" << std::endl;
            break;
        case 0xE000:
            switch (opcode & GET_8BITS) {
                case 0x009E:
                    std::cout << "Ex9E - SKP Vx" << std::endl;
                    break;
                case 0x00A1:
                    std::cout << "ExA1 - SKNP Vx" << std::endl;
                    break;
            }
        case 0xF000:
            std::cout << "Dropped in 0xF000" << std::endl;
            shouldDraw = true;
            break;

        default:
            printf("Unknowned opcode %s\n", opcode);
    }

    if (DT > 0) {
        --DT;
    }
    if (ST > 0) {
        --ST;
    }
}

void CPU::FetchOpcode() {
    opcode = Memory[PC] << 8 | Memory[PC + 1]; // get current instruction
    PC += 2;
}

int CPU::getVideoPitch() {
    return sizeof(Display[0] * VIDEO_WIDTH);
}
