#pragma once
#include <array>
#include <cstdint>
#include <vector>

namespace eclipse::assembler {
    namespace x86_64 {
        enum class Register64 : uint8_t {
            rax = 0, rcx  = 1, rdx  = 2, rbx  = 3,
            rsp = 4, rbp  = 5, rsi  = 6, rdi  = 7,
            r8  = 8, r9   = 9, r10  = 10, r11 = 11,
            r12 = 12, r13 = 13, r14 = 14, r15 = 15
        };

        enum class Register32 : uint8_t {
            eax  = 0, ecx   = 1, edx   = 2, ebx   = 3,
            esp  = 4, ebp   = 5, esi   = 6, edi   = 7,
            r8d  = 8, r9d   = 9, r10d  = 10, r11d = 11,
            r12d = 12, r13d = 13, r14d = 14, r15d = 15
        };

        enum class XmmRegister : uint8_t {
            xmm0  = 0, xmm1   = 1, xmm2   = 2, xmm3   = 3,
            xmm4  = 4, xmm5   = 5, xmm6   = 6, xmm7   = 7,
            xmm8  = 8, xmm9   = 9, xmm10  = 10, xmm11 = 11,
            xmm12 = 12, xmm13 = 13, xmm14 = 14, xmm15 = 15,
        };

        /// @brief movabs r64, imm64: Move a 64-bit immediate value to a register.
        constexpr std::array<uint8_t, 10> movabs(Register64 dst, uint64_t imm) {
            uint8_t rex = 0x48;
            if (static_cast<uint8_t>(dst) >= 8)
                rex |= 0x01;

            uint8_t opcode = 0xB8 + (static_cast<uint8_t>(dst) & 0x7);
            return {
                rex,
                opcode,
                static_cast<uint8_t>(imm & 0xFF),
                static_cast<uint8_t>((imm >> 8) & 0xFF),
                static_cast<uint8_t>((imm >> 16) & 0xFF),
                static_cast<uint8_t>((imm >> 24) & 0xFF),
                static_cast<uint8_t>((imm >> 32) & 0xFF),
                static_cast<uint8_t>((imm >> 40) & 0xFF),
                static_cast<uint8_t>((imm >> 48) & 0xFF),
                static_cast<uint8_t>((imm >> 56) & 0xFF),
            };
        }

        /// @brief mov r32, dword ptr [reg]: Load a 32-bit value from address stored in a register.
        constexpr std::array<uint8_t, 3> mov(Register32 dst, Register64 src) {
            uint8_t rex = 0x40;
            if (static_cast<uint8_t>(dst) >= 8)
                rex |= 0x04;
            if (static_cast<uint8_t>(src) >= 8)
                rex |= 0x01;

            uint8_t modrm = (static_cast<uint8_t>(dst) & 0x7) << 3;
            modrm |= (static_cast<uint8_t>(src) & 0x7);

            return {rex, 0x8B, modrm};
        }

        /// @brief jmp rel32: Jump to a relative address.
        constexpr std::array<uint8_t, 5> jmp(int32_t rel) {
            return {
                0xE9,
                static_cast<uint8_t>(rel & 0xFF),
                static_cast<uint8_t>((rel >> 8) & 0xFF),
                static_cast<uint8_t>((rel >> 16) & 0xFF),
                static_cast<uint8_t>((rel >> 24) & 0xFF)
            };
        }

        /// @brief movss xmm, dword ptr [r64]: Move scalar single-precision floating-point from memory to XMM register
        constexpr std::array<uint8_t, 5> movss(XmmRegister dst, Register64 src) {
            uint8_t rex = 0x40;
            if (static_cast<uint8_t>(dst) >= 8)
                rex |= 0x04; // REX.R bit for destination register extension
            if (static_cast<uint8_t>(src) >= 8)
                rex |= 0x01; // REX.B bit for source register extension

            uint8_t modrm = (static_cast<uint8_t>(dst) & 0x7) << 3;
            modrm |= (static_cast<uint8_t>(src) & 0x7);

            return {0xF3, rex, 0x0F, 0x10, modrm};
        }

        class Builder {
        public:
            Builder() = default;
            explicit Builder(uintptr_t baseAddress) : m_baseAddress(baseAddress) {}

            Builder& movabs(Register64 dst, uint64_t imm) {
                auto bytes = x86_64::movabs(dst, imm);
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                return *this;
            }

            Builder& mov(Register32 dst, Register64 src) {
                auto bytes = x86_64::mov(dst, src);
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                return *this;
            }

            Builder& jmp(int32_t offset, bool relative = false) {
                // If relative is true, offset is the absolute address, and we calculate the relative jump.
                if (relative) offset -= m_baseAddress + m_bytes.size() + 5; // 5 is the size of the jmp instruction
                auto bytes = x86_64::jmp(offset);
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                return *this;
            }

            Builder& nop(size_t count = 1) {
                m_bytes.insert(m_bytes.end(), count, 0x90);
                return *this;
            }

            Builder& movss(XmmRegister dst, Register64 src) {
                auto bytes = x86_64::movss(dst, src);
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                return *this;
            }

            std::vector<uint8_t> build() { return std::move(m_bytes); }

        private:
            uintptr_t m_baseAddress = 0;
            std::vector<uint8_t> m_bytes;
        };
    }

    namespace arm64 {
        enum class Register {
            x0  = 0b00000, w0  = 0b100000,
            x1  = 0b00001, w1  = 0b100001,
            x2  = 0b00010, w2  = 0b100010,
            x3  = 0b00011, w3  = 0b100011,
            x4  = 0b00100, w4  = 0b100100,
            x5  = 0b00101, w5  = 0b100101,
            x6  = 0b00110, w6  = 0b100110,
            x7  = 0b00111, w7  = 0b100111,
            x8  = 0b01000, w8  = 0b101000,
            x9  = 0b01001, w9  = 0b101001,
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

        enum class FloatRegister {
            s0  = 0b00000, d0  = 0b100000,
            s1  = 0b00001, d1  = 0b100001,
            s2  = 0b00010, d2  = 0b100010,
            s3  = 0b00011, d3  = 0b100011,
            s4  = 0b00100, d4  = 0b100100,
            s5  = 0b00101, d5  = 0b100101,
            s6  = 0b00110, d6  = 0b100110,
            s7  = 0b00111, d7  = 0b100111,
            s8  = 0b01000, d8  = 0b101000,
            s9  = 0b01001, d9  = 0b101001,
            s10 = 0b01010, d10 = 0b101010,
            s11 = 0b01011, d11 = 0b101011,
            s12 = 0b01100, d12 = 0b101100,
            s13 = 0b01101, d13 = 0b101101,
            s14 = 0b01110, d14 = 0b101110,
            s15 = 0b01111, d15 = 0b101111,
            s16 = 0b10000, d16 = 0b110000,
            s17 = 0b10001, d17 = 0b110001,
            s18 = 0b10010, d18 = 0b110010,
            s19 = 0b10011, d19 = 0b110011,
            s20 = 0b10100, d20 = 0b110100,
            s21 = 0b10101, d21 = 0b110101,
            s22 = 0b10110, d22 = 0b110110,
            s23 = 0b10111, d23 = 0b110111,
            s24 = 0b11000, d24 = 0b111000,
            s25 = 0b11001, d25 = 0b111001,
            s26 = 0b11010, d26 = 0b111010,
            s27 = 0b11011, d27 = 0b111011,
            s28 = 0b11100, d28 = 0b111100,
            s29 = 0b11101, d29 = 0b111101,
            s30 = 0b11110, d30 = 0b111110,
            s31 = 0b11111, d31 = 0b111111
        };

        constexpr bool is_w(Register reg) { return static_cast<int>(reg) & 0b100000; }
        constexpr int operator&(Register dst, int rhs) { return static_cast<int>(dst) & rhs; }
        constexpr int operator|(int lhs, Register dst) { return lhs | static_cast<int>(dst); }

        constexpr bool is_d(FloatRegister reg) { return static_cast<int>(reg) & 0b100000; }
        constexpr int operator&(FloatRegister dst, int rhs) { return static_cast<int>(dst) & rhs; }
        constexpr int operator|(int lhs, FloatRegister dst) { return lhs | static_cast<int>(dst); }

        /// @brief Form PC Relative Address:
        /// Adds an immediate that is shifted left by 12 bits to the PC value.
        constexpr std::array<uint8_t, 4> adrp(Register dst, uint64_t addr) {
            uint32_t page = static_cast<uint32_t>(addr >> 12);
            uint32_t immlo = page & 0x3;
            uint32_t immhi = (page >> 2) & 0x7FFFF;
            uint8_t rd = dst & 0x1F;

            return {
                static_cast<uint8_t>(rd | ((immhi & 0x7) << 5)),
                static_cast<uint8_t>((immhi >> 3) & 0xFF),
                static_cast<uint8_t>((immhi >> 11) & 0xFF),
                static_cast<uint8_t>(0x90 | ((immlo & 0x3) << 5))
            };
        }

        /// @brief Add
        /// Adds an immediate value to a register and stores the result in a destination register.
        constexpr std::array<uint8_t, 4> add(Register dst, Register src, uint64_t imm) {
            uint8_t rn = src & 0b11111;
            uint8_t sf = is_w(dst) ? 0 : 1;
            uint8_t sh = 0;
            uint16_t imm12;

            if (imm <= 0xFFF) {
                // number fits in 12 bits
                imm12 = static_cast<uint16_t>(imm);
                sh = 0;
            } else if ((imm & 0xFFF) == 0 && (imm >> 12) <= 0xFFF) {
                // number can be represented as a 12-bit immediate with a shift
                imm12 = static_cast<uint16_t>(imm >> 12);
                sh = 1;
            } else {
                // number doesn't fit, idk just use the lower 12 bits for now
                imm12 = static_cast<uint16_t>(imm & 0xFFF);
                sh = 0;
            }

            return {
                static_cast<uint8_t>((dst & 0b11111) | (rn << 5)),
                static_cast<uint8_t>((rn >> 3) | ((imm12 & 0x3F) << 2)),
                static_cast<uint8_t>(((imm12 >> 6) & 0x3F) | (sh << 6)),
                static_cast<uint8_t>(0x91 | (sf << 7))
            };
        }

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

        /// @brief Load SIMD&FP Register with Immediate Offset:
        /// LDR <St>, [<Xn|SP>{, #<pimm>}] - 32-bit variant
        /// LDR <Dt>, [<Xn|SP>{, #<pimm>}] - 64-bit variant
        constexpr std::array<uint8_t, 4> ldr(FloatRegister rt, Register rn, uint16_t imm = 0) {
            uint8_t size, opc;
            uint8_t scale;
            if (is_d(rt)) {
                // Double precision (D register) - 64-bit
                size = 0b11;
                opc = 0b01;
                scale = 3;
            } else {
                // Single precision (S register) - 32-bit
                size = 0b10;
                opc = 0b01;
                scale = 2;
            }

            uint16_t imm12 = imm >> scale;
            uint8_t rt_bits = rt & 0b11111;
            uint8_t rn_bits = rn & 0b11111;

            return {
                static_cast<uint8_t>(rt_bits | ((rn_bits & 0b111) << 5)),
                static_cast<uint8_t>((rn_bits >> 3) | ((imm12 & 0b1111111) << 2)),
                static_cast<uint8_t>(0b01000000 | ((imm12 >> 7) & 0b11111)),
                static_cast<uint8_t>((size << 6) | 0b111000 | (opc << 2) | 0b1)
            };
        }

        /// @brief Load General Purpose Register with Immediate Offset:
        /// LDR <Wt>, [<Xn|SP>{, #<pimm>}] - 32-bit variant
        /// LDR <Xt>, [<Xn|SP>{, #<pimm>}] - 64-bit variant
        constexpr std::array<uint8_t, 4> ldr(Register rt, Register rn, uint16_t imm = 0) {
            uint8_t size, opc;
            uint8_t scale;

            if (is_w(rt)) {
                // 32-bit W register
                size = 0b10;
                opc = 0b00;
                scale = 2;
            } else {
                // 64-bit X register
                size = 0b11;
                opc = 0b00;
                scale = 3;
            }

            uint16_t imm12 = imm >> scale;
            uint8_t rt_bits = rt & 0b11111;
            uint8_t rn_bits = rn & 0b11111;

            return {
                static_cast<uint8_t>(rt_bits | ((rn_bits & 0b111) << 5)),
                static_cast<uint8_t>((rn_bits >> 3) | ((imm12 & 0b1111111) << 2)),
                static_cast<uint8_t>(0b01000000 | ((imm12 >> 7) & 0b11111)),
                static_cast<uint8_t>((size << 6) | 0b111000 | (opc << 2) | 0b1)
            };
        }

        /// @brief Branch with Immediate:
        /// Branch to a relative address, with a signed 26-bit immediate value.
        constexpr std::array<uint8_t, 4> b(int32_t offset) {
            int32_t word_offset = offset >> 2;
            uint32_t imm26 = static_cast<uint32_t>(word_offset) & 0x3FFFFFF;
            uint32_t instr = (0b000101 << 26) | imm26;

            return {
                static_cast<uint8_t>(instr & 0xFF),
                static_cast<uint8_t>((instr >> 8) & 0xFF),
                static_cast<uint8_t>((instr >> 16) & 0xFF),
                static_cast<uint8_t>((instr >> 24) & 0xFF)
            };
        }

        /// @brief No Operation:
        /// Instruction that does nothing, used for padding.
        constexpr std::array<uint8_t, 4> nop() {
            return {0x1F, 0x20, 0x03, 0xD5};
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

        class Builder {
        public:
            constexpr Builder() = default;
            explicit constexpr Builder(uintptr_t baseAddress) : m_baseAddress(baseAddress) {}

            constexpr Builder& adrp(Register dst, uint64_t addr) {
                auto bytes = arm64::adrp(dst, addr);
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                return *this;
            }

            constexpr Builder& add(Register dst, Register src, uint64_t imm) {
                auto bytes = arm64::add(dst, src, imm);
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                return *this;
            }

            constexpr Builder& movz(Register dst, uint16_t imm, uint8_t shift = 0) {
                auto bytes = arm64::movz(dst, imm, shift);
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                return *this;
            }

            constexpr Builder& movk(Register dst, uint16_t imm, uint8_t shift = 0) {
                auto bytes = arm64::movk(dst, imm, shift);
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                return *this;
            }

            constexpr Builder& mov(Register dst, uint64_t imm) {
                auto bytes = arm64::movz(dst, static_cast<uint16_t>(imm & 0xFFFF));
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                if (imm > 0xFFFF) {
                    bytes = arm64::movk(dst, static_cast<uint16_t>((imm >> 16) & 0xFFFF), 16);
                    m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                }
                if (imm > 0xFFFFFFFF) {
                    bytes = arm64::movk(dst, static_cast<uint16_t>((imm >> 32) & 0xFFFF), 32);
                    m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                }
                if (imm > 0xFFFFFFFFFFFF) {
                    bytes = arm64::movk(dst, static_cast<uint16_t>((imm >> 48) & 0xFFFF), 48);
                    m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                }
                return *this;
            }

            constexpr Builder& ldr(FloatRegister rt, Register rn, uint16_t imm = 0) {
                auto bytes = arm64::ldr(rt, rn, imm);
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                return *this;
            }

            constexpr Builder& ldr(Register rt, Register rn, uint16_t imm = 0) {
                auto bytes = arm64::ldr(rt, rn, imm);
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                return *this;
            }

            constexpr Builder& b(int32_t offset, bool relative = false) {
                // If relative is true, offset is the absolute address, and we calculate the relative jump.
                if (relative) offset -= m_baseAddress + m_bytes.size() + 4;
                auto bytes = arm64::b(offset);
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                return *this;
            }

            constexpr Builder& nop(size_t count = 1) {
                for (size_t i = 0; i < count; ++i) {
                    auto bytes = arm64::nop();
                    m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                }
                return *this;
            }

            /// @brief Adds NOP instructions to make the total size of bytes equal to `byteCount`.
            constexpr Builder& pad_nops(size_t byteCount) {
                size_t currentSize = m_bytes.size();
                if (currentSize < byteCount) {
                    size_t nopsToAdd = (byteCount - currentSize + 3) / 4; // Each NOP is 4 bytes
                    this->nop(nopsToAdd);
                }
                return *this;
            }

            constexpr std::vector<uint8_t> build() { return std::move(m_bytes); }

            template <size_t N>
            consteval std::array<uint8_t, N> build_array() {
                if (m_bytes.size() != N) throw std::runtime_error("Size mismatch!");

                std::array<uint8_t, N> result{};
                std::copy(m_bytes.begin(), m_bytes.end(), result.begin());
                return result;
            }

        private:
            uintptr_t m_baseAddress = 0;
            std::vector<uint8_t> m_bytes;
        };
    }

    namespace armv7 {
        enum class Register : uint8_t {
            r0  = 0, r1  = 1, r2  = 2, r3   = 3,
            r4  = 4, r5  = 5, r6  = 6, r7   = 7,
            r8  = 8, r9  = 9, r10 = 10, r11 = 11,
            r12 = 12, sp = 13, lr = 14, pc  = 15
        };

        /// @brief NOP instruction for Thumb-2 (ARMv7). 2 bytes.
        constexpr std::array<uint8_t, 2> nop_t() {
            return {0x00, 0xBF};
        }

        /// @brief NOP instruction for ARMv7. 4 bytes.
        constexpr std::array<uint8_t, 4> nop() {
            return {0x00, 0x00, 0xA0, 0xE3};
        }

        /// @brief MOV immediate (Thumb-2, 32-bit instruction)
        /// MOVW <Rd>, #<imm16> - Move 16-bit immediate to register
        constexpr std::array<uint8_t, 4> movw(Register rd, uint16_t imm16) {
            uint8_t rd_bits = static_cast<uint8_t>(rd) & 0xF;
            uint8_t imm4 = (imm16 >> 12) & 0xF;
            uint8_t i = (imm16 >> 11) & 1;
            uint8_t imm3 = (imm16 >> 8) & 0x7;
            uint8_t imm8 = imm16 & 0xFF;

            return {
                static_cast<uint8_t>(0x40 | imm4),
                static_cast<uint8_t>(0xF2 | (i << 2)),
                imm8,
                static_cast<uint8_t>((imm3 << 4) | rd_bits),
            };
        }

        /// @brief MOVT (Move Top) - Move 16-bit immediate to top half of register (Thumb-2, 32-bit instruction)
        /// MOVT <Rd>, #<imm16> - Move immediate to bits [31:16], keeping bits [15:0]
        constexpr std::array<uint8_t, 4> movt(Register rd, uint16_t imm16) {
            uint8_t rd_bits = static_cast<uint8_t>(rd) & 0xF;
            uint8_t imm4 = (imm16 >> 12) & 0xF;
            uint8_t i = (imm16 >> 11) & 1;
            uint8_t imm3 = (imm16 >> 8) & 0x7;
            uint8_t imm8 = imm16 & 0xFF;

            return {
                static_cast<uint8_t>(0xC0 | imm4),
                static_cast<uint8_t>(0xF2 | (i << 2)),
                imm8,
                static_cast<uint8_t>((imm3 << 4) | rd_bits),
            };
        }

        /// @brief LDR immediate (Thumb, 16-bit instruction)
        /// LDR <Rt>, [<Rn>] - Load register from memory
        constexpr std::array<uint8_t, 2> ldr(Register rt, Register rn) {
            uint8_t rt_bits = static_cast<uint8_t>(rt) & 0x7;
            uint8_t rn_bits = static_cast<uint8_t>(rn) & 0x7;

            return {
                static_cast<uint8_t>(rn_bits << 3),
                static_cast<uint8_t>(0x68 | rt_bits)
            };
        }

        class Builder {
        public:
            Builder() = default;
            explicit Builder(uintptr_t baseAddress) : m_baseAddress(baseAddress) {}

            Builder& mov(Register dst, uint32_t imm) {
                auto bytes = armv7::movw(dst, static_cast<uint16_t>(imm & 0xFFFF));
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                if (imm > 0xFFFF) {
                    bytes = armv7::movt(dst, static_cast<uint16_t>((imm >> 16) & 0xFFFF));
                    m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                }
                return *this;
            }

            Builder& ldr_t(Register rt, Register rn, uint16_t imm = 0) {
                auto bytes = armv7::ldr(rt, rn);
                m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                return *this;
            }

            Builder& nop(size_t count = 1) {
                for (size_t i = 0; i < count; ++i) {
                    auto bytes = armv7::nop();
                    m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                }
                return *this;
            }

            Builder& nop_t(size_t count = 1) {
                for (size_t i = 0; i < count; ++i) {
                    auto bytes = armv7::nop_t();
                    m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
                }
                return *this;
            }

            std::vector<uint8_t> build() { return std::move(m_bytes); }

        private:
            uintptr_t m_baseAddress = 0;
            std::vector<uint8_t> m_bytes;
        };
    }
}
