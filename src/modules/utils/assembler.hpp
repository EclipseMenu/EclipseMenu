#pragma once

namespace eclipse::assembler {
namespace arm64 {
enum class Register {
    x0 = 0b00000, w0 = 0b100000,
    x1 = 0b00001, w1 = 0b100001,
    x2 = 0b00010, w2 = 0b100010,
    x3 = 0b00011, w3 = 0b100011,
    x4 = 0b00100, w4 = 0b100100,
    x5 = 0b00101, w5 = 0b100101,
    x6 = 0b00110, w6 = 0b100110,
    x7 = 0b00111, w7 = 0b100111,
    x8 = 0b01000, w8 = 0b101000,
    x9 = 0b01001, w9 = 0b101001,
    x10 = 0b01010, w10 = 0b101010,
    x11 = 0b01011, w11 = 0b101011,
    x12 = 0b01100, w12 = 0b101100,
    x13 = 0b01101, w13 = 0b101101,
    x14 = 0b01110, w14 = 0b101110,
    x15 = 0b01111, w15 = 0b101111,
    x16 = 0b10000, w16 = 0b110000,
    x17 = 0b10001, w17 = 0b110001,
    x18 = 0b10010, w18 = 0b110010,
    x19 = 0b10011, w19 = 0b110011,
    x20 = 0b10100, w20 = 0b110100,
    x21 = 0b10101, w21 = 0b110101,
    x22 = 0b10110, w22 = 0b110110,
    x23 = 0b10111, w23 = 0b110111,
    x24 = 0b11000, w24 = 0b111000,
    x25 = 0b11001, w25 = 0b111001,
    x26 = 0b11010, w26 = 0b111010,
    x27 = 0b11011, w27 = 0b111011,
    x28 = 0b11100, w28 = 0b111100,
    x29 = 0b11101, w29 = 0b111101,
    x30 = 0b11110, w30 = 0b111110,
    x31 = 0b11111, w31 = 0b111111,

    pc = 0b11110,
    sp = 0b11111
};

constexpr bool is_w(Register reg) { return static_cast<int>(reg) & 0b100000; }
constexpr int operator&(Register dst, int rhs) { return static_cast<int>(dst) & rhs; }
constexpr int operator|(int lhs, Register dst) { return lhs | static_cast<int>(dst); }

/// @brief Move with Zero:
/// Move a 16-bit immediate value to a register, with a left shift.
constexpr std::array<uint8_t, 4> movz(Register dst, uint16_t imm, uint8_t shift = 0) {
    uint8_t sf = is_w(dst) ? 0 : 1;
    uint8_t hw = shift / 16;

    return {
        static_cast<uint8_t>(((imm & 0b111) << 5) | (dst & 0b11111)),
        static_cast<uint8_t>(imm >> 3),
        static_cast<uint8_t>(0b10000000 | (hw << 5) | imm >> 11),
        static_cast<uint8_t>((sf << 7) | 0b1010010)
    };
}

/// @brief Move wide with keep:
/// Move a 16-bit immediate value to a register, with a left shift, keeping the rest of the register.
constexpr std::array<uint8_t, 4> movk(Register dst, uint16_t imm, uint8_t shift = 0) {
    uint8_t sf = is_w(dst) ? 0 : 1;
    uint8_t hw = shift / 16;

    return {
        static_cast<uint8_t>(((imm & 0b111) << 5) | (dst & 0b11111)),
        static_cast<uint8_t>(imm >> 3),
        static_cast<uint8_t>(0b10000000 | (hw << 5) | imm >> 11),
        static_cast<uint8_t>((sf << 7) | 0b1110010)
    };
}

/// @brief Helper function to move a float to a register as an immediate value. Generates 2 instructions.
[[nodiscard]] inline std::array<uint8_t, 8> mov_float(Register dst, float imm) {
    auto bytes = reinterpret_cast<uint8_t*>(&imm);
    auto instr1 = movz(dst, *reinterpret_cast<uint16_t*>(bytes));
    auto instr2 = movk(dst, *reinterpret_cast<uint16_t*>(bytes + 2), 16);
    return {
        instr1[0], instr1[1], instr1[2], instr1[3],
        instr2[0], instr2[1], instr2[2], instr2[3]
    };
}

/// @brief Helper function to move a double value to a register as an immediate value. Generates 3 instructions.
[[nodiscard]] inline std::array<uint8_t, 12> mov_double(Register dst, double imm) {
    uint64_t bits = *reinterpret_cast<uint64_t*>(&imm);
    auto instr1 = movz(dst, static_cast<uint16_t>(bits >> 16), 16);
    auto instr2 = movk(dst, static_cast<uint16_t>(bits >> 32), 32);
    auto instr3 = movk(dst, static_cast<uint16_t>(bits >> 48), 48);
    return {
        instr1[0], instr1[1], instr1[2], instr1[3],
        instr2[0], instr2[1], instr2[2], instr2[3],
        instr3[0], instr3[1], instr3[2], instr3[3]
    };
}
}
}