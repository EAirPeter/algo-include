#pragma once

#include "Common.hpp"

namespace {
    inline int Log2(U64 N) {
        static const U8 Table[64] = {
            0,  58, 1,  59, 47, 53, 2,  60, 39, 48, 27, 54, 33, 42, 3,  61,
            51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4,  62,
            57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21, 56,
            45, 25, 31, 35, 16, 9,  12, 44, 24, 15, 8,  23, 7,  6,  5,  63,
        };
        N |= N >> 1;
        N |= N >> 2;
        N |= N >> 4;
        N |= N >> 8;
        N |= N >> 16;
        N |= N >> 32;
        return (int) Table[(N * 0x03f6eaf2cd271461u) >> 58];
    }
    inline int Log2Ceil(U64 N) {
        return Log2((N << 1) - 1);
    }
}
