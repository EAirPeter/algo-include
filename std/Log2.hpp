#pragma once

#include "Common.hpp"

namespace {
    inline int Log2(U32 N) NOEX {
        static CEXV U8 Table[32] = {
            0,  9,  1,  10, 13, 21, 2,  29,
            11, 14, 16, 18, 22, 25, 3,  30,
            8,  12, 20, 28, 15, 17, 24, 7,
            19, 27, 23, 6,  26, 5,  4,  31,
        };
        N |= N >> 1;
        N |= N >> 2;
        N |= N >> 4;
        N |= N >> 8;
        N |= N >> 16;
        return (int) Table[(N * 0x07c4acddu) >> 27];
    }
    inline int Log2(U64 N) NOEX {
        static CEXV U8 Table[64] = {
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
    template<class Uns>
    inline int Log2C(Uns N) NOEX {
        return Log2((N << 1) - 1);
    }
}
