//
// Created by Miguel Toledano on 09/10/22.
//
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <random>

#include "cpu.h"
#include "masks.h"

int randByte;
int x;
int y;
int h;
int pixels;
bool key_pressed = false;

uint8_t FONTSET[FONTSET_SIZE]{
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
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
    opcode = 0;
    std::memset(Memory, 0, MEMORY_SIZE);
    std::memset(V, 0, REGISTERS_SIZE);
    std::memset(S, 0, STACK_SIZE);
    std::memset(Keyboard, 0, KEYPAD_SIZE);
    std::memset(Display, 0, VIDEO_WIDTH * VIDEO_HEIGHT);

    I = 0;
    PC = START_ADDRESS;
    DT = 0;
    ST = 0;
    SP = 0;

    // Load fonts into memory
    std::memcpy(Memory + 0x50, FONTSET, FONTSET_SIZE);
}

bool CPU::loadRom(const std::string &filename) {
    bool success = true;
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    std::vector<uint8_t> bytes(
            (std::istreambuf_iterator<char>(file)),
            (std::istreambuf_iterator<char>()));

    if (bytes.size() > MAX_FILE_SIZE) {
        throw std::runtime_error("File size is bigger than max rom size.");
    } else if (bytes.empty()) {
        throw std::runtime_error("No file or empty file.");
    }

    std::memcpy(Memory + START_ADDRESS, bytes.data(), bytes.size());

    return success;
}

void CPU::Cycle() {
    FetchOpcode();

    switch (opcode & GET_HIGH_NIBBLE) {
        case 0x0000:
            switch (opcode & GET_8BITS) {
                case 0x00E0:
                    // CLS
                    std::cout << "CLS opcode " << std::hex << opcode << std::endl;
                    // Clears display
                    std::memset(Display, 0, VIDEO_WIDTH * VIDEO_HEIGHT);
                    shouldDraw = true;
                    break;
                case 0x00EE:
                    // RET
                    std::cout << "RET opcode " << std::hex << opcode << std::endl;
                    // Return from a subroutine
                    PC = S[PC];
                    SP--;
                    break;
                default:
                    std::cout << "Invalid opcode for 0x0000. Opcode: " << opcode << std::endl;
                    break;
            }
        case 0x1000:
            // JP addr
            std::cout << "JP opcode " << std::hex << opcode << std::endl;
            // Jump to location nnn
            PC = opcode & GET_ADDRESS;
            break;
        case 0x2000:
            // CALL addr
            std::cout << "CALL opcode " << std::hex << opcode << std::endl;
            // Call subroutine at nnn
            SP++;
            S[SP] = PC;
            PC = opcode & GET_ADDRESS;
            break;
        case 0x3000:
            // SE Vx, byte
            // Skip next instruction if Vx = kk
            std::cout << "0x300 - SE Vx, byte. Opcode: " << std::hex << opcode << std::endl;
            if (V[(opcode & GET_X) >> 8] == (opcode & GET_8BITS)) {
                PC += 2;
            }
            break;
        case 0x4000:
            // SNE Vx, byte
            // Skip next instruction if Vx != kk
            std::cout << "0x4000 - SNE Vx, byte: " << std::hex << opcode << std::endl;
            if (V[(opcode & GET_X) >> 8] != (opcode & GET_8BITS)) {
                PC += 2;
            }
            break;
        case 0x5000:
            // SE Vx, Vy
            // Skip next instruction if Vx = Vy
            std::cout << "0x5000 - SE Vx, Vy: " << std::hex << opcode << std::endl;
            if (V[(opcode & GET_X) >> 8] == V[(opcode & GET_Y) >> 4]) {
                PC += 2;
            }
            break;
        case 0x6000:
            // LD Vx, byte
            // Set Vx = kk
            std::cout << "0x6000 - LD Vx, byte: " << std::hex << opcode << std::endl;
            V[(opcode & GET_X) >> 8] = opcode & GET_8BITS;
            break;
        case 0x7000:
            // ADD Vx, byte
            // Set Vx = Vx + kk
            std::cout << "0x7000 - ADD Vx, byte: " << std::hex << opcode << std::endl;
            V[(opcode & GET_X) >> 8] += opcode & GET_8BITS;
            break;
        case 0x8000:
            switch (opcode & GET_4BITS) {
                case 0x0000:
                    // LD Vx, Vy
                    // Set Vx = Vy
                    std::cout << "0x8XY0 - LD Vx, Vy: " << std::hex << opcode << std::endl;
                    V[(opcode & GET_X) >> 8] = V[(opcode & GET_Y) >> 4];
                    break;
                case 0x0001:
                    // OR Vx, Vy
                    // Set Vx = Vx OR Vy
                    std::cout << "0x8001 - OR Vx, Vy: " << std::hex << opcode << std::endl;
                    V[(opcode & GET_X) >> 8] |= V[(opcode & GET_Y) >> 4];
                    break;
                case 0x0002:
                    // AND Vx, Vy
                    // Set Vx = Vx AND Vy
                    std::cout << "0x8002 - AND Vx, Vy: " << std::hex << opcode << std::endl;
                    V[(opcode & GET_X) >> 8] &= V[(opcode & GET_Y) >> 4];
                    break;
                case 0x0003:
                    // XOR Vx, Vy
                    // Set Vx = Vx XOR Vy
                    std::cout << "0x8003 - XOR Vx, Vy: " << std::hex << opcode << std::endl;
                    V[(opcode & GET_X) >> 8] ^= V[(opcode & GET_Y) >> 4];
                    break;
                case 0x0004:
                    // ADD Vx, Vy
                    // Set Vx = Vx + Vy, set VF = carry
                    std::cout << "0x8004 - ADD Vx, Vy: " << std::hex << opcode << std::endl;

                    V[0xF] = 0;

                    if (V[(opcode & GET_X) >> 8] + V[(opcode & GET_Y) >> 4] > 0xFF) {
                        V[0xF] = 1;
                    }

                    V[(opcode & GET_X) >> 8] = (V[(opcode & GET_X) >> 8] + V[(opcode & GET_Y) >> 4]) & GET_8BITS;
                    break;
                case 0x0005:
                    // SUB Vx, Vy
                    // Set Vx = Vx - Vy, set VF = NOT borrow
                    std::cout << "0x8005 - SUB Vx, Vy: " << std::hex << opcode << std::endl;
                    V[0xF] = V[(opcode & GET_X) >> 8] > V[(opcode & GET_Y) >> 4] ? 1 : 0;
                    V[(opcode & GET_X) >> 8] -= V[(opcode & GET_Y) >> 4];
                    break;
                case 0x0006:
                    // SHR Vx {, Vy}
                    // Set Vx = Vx SHR 1
                    std::cout << "0x8006 - SHR Vx {, Vy}: " << std::hex << opcode << std::endl;
                    V[0xF] = 0;

                    if ((V[(opcode & GET_X) >> 8] & 0x01) == 0x01) {
                        V[0xF] = 1;
                    }
                    V[(opcode & GET_X) >> 8] /= 2;
                    break;
                case 0x0007:
                    // SUBN Vx, Vy
                    // Set Vx = Vy - Vx, set VF - NOT borrow
                    std::cout << "0x8007 SUB Vx, Vy: " << std::hex << opcode << std::endl;
                    V[0xF] = V[(opcode & GET_X) >> 8] > V[(opcode & GET_Y) >> 4] ? 1 : 0;
                    V[(opcode & GET_X) >> 8] = V[(opcode & GET_Y) >> 4] - V[(opcode & GET_X) >> 8];
                    break;
                case 0x000E:
                    // SHL Vx {, Vy}
                    // Set Vx = Vx SHL 1.
                    std::cout << "0x800E SHL Vx {, Vy}: " << std::hex << opcode << std::endl;
                    V[0xF] = 0;
                    if ((V[(opcode & GET_X) >> 8] & 0x10) == 0x10) {
                        V[0xF] = 1;
                    }
                    V[(opcode & GET_X) >> 8] *= 2;
                    break;
                default:
                    std::cout << "Invalid opcode 0x8000: " << std::hex << opcode << std::endl;
            }
        case 0x9000:
            // SNE Vx, Vy
            // Skip next instruction if Vx != Vy
            std::cout << "0x9000 SNE Vx, Vy: " << std::hex << opcode << std::endl;
            if (V[(opcode & GET_X) >> 8] != V[(opcode & GET_Y) >> 4]) {
                PC += 2;
            }
            break;
        case 0xA000:
            // LD I, addr
            // Set I = nnn
            std::cout << "0xA000 LD I, " << std::hex << opcode << std::endl;
            I = opcode & GET_ADDRESS;
            break;
        case 0xB000:
            // JP V0, addr
            // Jump to location nnn + V0
            std::cout << "0xB000 JP V0, addr: " << std::hex << opcode << std::endl;
            PC = (opcode & GET_ADDRESS) + V[0x0];
            break;
        case 0xC000:
            // RND Vx, byte
            // Set Vx = random byte AND kk
            std::cout << "0xC000 RND Vx, byte: " << std::hex << opcode << std::endl;
            randByte = randomByte();
            V[(opcode & GET_X) >> 8] = randByte & (opcode & GET_8BITS);
            break;
        case 0xD000:
            // DRW Vx, Vy, nibble
            // Display n-byte sprite starting at memory location I
            // at (Vx, Vy), set VF = collision.
            std::cout << "0xD000 DRW Vx, Vy, nibble: " << std::hex << opcode << std::endl;
            x = V[(opcode & GET_X) >> 8];
            y = V[(opcode & GET_Y) >> 4];
            h = V[(opcode & GET_4BITS)];

            V[0xF] = 0;

            for (int row = 0; row < h; row++) {
                pixels = Memory[I + row];

                for (int col = 0; col < 8; col++) {
                    // If the but (sprite) is not 0, render/erase the pixel
                    if ((pixels & (0x80) >> row) != 0) {
                        if (Display[(x + row + (y + col) * 64)] == 1) {
                            V[0xF] = 1;
                        }
                        Display[x + row + ((y + col) * 64)] ^= 1;
                    }
                }
            }

            shouldDraw = true;
            break;
        case 0xE000:
            switch (opcode & GET_8BITS) {
                case 0x009E:
                    // SKP Vx
                    // Skip next instruction if key with the value of Vx is pressed
                    std::cout << "0xEx9E SKP Vx: " << std::hex << opcode << std::endl;
                    x = V[(opcode & GET_X) >> 8];

                    if (Keyboard[x] != 0) {
                        PC += 2;
                    }
                    break;
                case 0x00A1:
                    // SKNP Vx
                    // Skip next instruction if key with the value of Vx is not pressed
                    std::cout << "0xExA1 SKP Vx: " << std::hex << opcode << std::endl;
                    x = V[(opcode & GET_X) >> 8];
                    if (Keyboard[x] == 0) {
                        PC += 2;
                    }
                    break;
                default:
                    std::cout << "Invalid opcode for 0xE000: " << std::hex << opcode << std::endl;
                    break;
            }
        case 0xF000:
            switch (opcode & GET_8BITS) {
                case 0x0007:
                    // LD Vx, DT
                    // Set Vx = Delay timer value
                    std::cout << "0xFx07 LD Vx, DT: " << std::hex << opcode << std::endl;
                    V[(opcode & GET_X) >> 8] = DT;
                    break;
                case 0x000A:
                    // LD Vx, K
                    // Wait for a key press, store the value of the key in Vx
                    std::cout << "0xFx0A LD Vx, K: " << std::hex << opcode << std::endl;
                    for (int i = 0; i < KEYPAD_SIZE; i++) {
                        if (Keyboard[i] != 0) {
                            V[(opcode & GET_X) >> 8] = i;
                            key_pressed = true;
                        }
                    }

                    if (!key_pressed) {
                        return;
                    }
                    break;
                case 0x0015:
                    // LD DT, Vx
                    // Set delay timer = Vx
                    std::cout << "0xFx15 LD DT, Vx: " << std::hex << opcode << std::endl;
                    DT = V[(opcode & GET_X) >> 8];
                    break;
                case 0x0018:
                    // LD ST, Vx
                    // Set Sound Timer = Vx
                    std::cout << "0xFx18 LD ST, Vx: " << std::hex << opcode << std::endl;
                    ST = V[(opcode & GET_X) >> 8];
                    break;
                case 0x001E:
                    // ADD I, Vx
                    // SET I = I + Vx
                    std::cout << "0xFx1E ADD I, Vx: " << std::hex << opcode << std::endl;
                    I += V[(opcode & GET_X) >> 8];
                    break;
                case 0x0029:
                    // LD F, Vx
                    // Set I = location of sprite for digit Vx
                    std::cout << "0xFx29 LD F, Vx: " << std::hex << opcode << std::endl;
                    I = V[(opcode & GET_X) >> 8] * 0x5;
                    break;
                case 0x0033:
                    // LD B, Vx
                    // Store BCD representation Of Vx in memory locations I, I+1 and I+2
                    std::cout << "0xFx33 LD B, Vx: " << std::hex << opcode << std::endl;
                    // hundreds I
                    Memory[I] = V[(opcode & GET_X) >> 8] / 100;
                    // tens I+1
                    Memory[I + 1] = (V[(opcode & GET_X) >> 8] / 10) % 10;
                    // ones digit I+2
                    Memory[I + 2] = V[(opcode & GET_X) >> 8] / 10;

                    break;
                case 0x0055:
                    // LD [I], Vx
                    // Store the registers V0 through Vx in memory starting at location I
                    std::cout << "0xF055 LD [I], Vx: " << std::hex << opcode << std::endl;
                    for (int i = 0; i <= ((opcode & GET_X) >> 8); i++) {
                        Memory[I + i] = V[i];
                    }

                    I += ((opcode & GET_X) >> 8) + 1;
                    break;
                case 0x0065:
                    // LD Vx. [I]
                    // Read registers V0 through Vx from memory starting at location I
                    for (int i = 0; i <= ((opcode & GET_X) >> 8); i++) {
                        V[i] = Memory[I + i];
                    }

                    I += ((opcode & GET_X) >> 8) + 1;
                    break;
                default:
                    std::cout << "Unknown opcode for 0xF - " << std::hex << opcode << std::endl;
                    break;
            }
        default:
            std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
            break;
    }

    // Timers

    if (DT > 0) {
        --DT;
    }

    if (ST > 0) {
        if (ST == 1) {
            std::cout << "BEEEEP!" << std::endl;
            playSound = true;
        }
        --ST;
    }
    // end timers
}

void CPU::FetchOpcode() {
    opcode = Memory[PC] << 8 | Memory[PC + 1];
    PC += 2;
}

int CPU::randomByte() {
    std::random_device seeder;
    std::ranlux48 gen(seeder());
    std::uniform_int_distribution<int> uniform_0_255(0, 255);
    return uniform_0_255(gen);
}
