// Miscellaneous Utilities
#pragma once

#include <cstdint>
#include <type_traits>

// Return: Absolute value of N
template<class Integral>
constexpr ::std::make_unsigned_t<Integral> Abs(Integral N) noexcept {
    using Unsigned = ::std::make_unsigned_t<Integral>;
    using Signed = ::std::make_signed_t<Integral>;
    return  (Signed) N < 0 ? (Unsigned) -(Signed) N : (Unsigned) N;
}

// Require: No overflow
// Return: B-th power of A
template<class Arith, class Unsigned>
constexpr Arith Pow(Arith A, Unsigned B) noexcept {
    Arith res(1);
    while (B) {
        if (B & 1)
            res *= A;
        A *= A;
        B >>= 1;
    }
    return res;
}

// Impl: Extract all factor P from N
template<class Unsigned, class Count>
constexpr Count ImplFactorize(Unsigned& N, Unsigned P) noexcept {
    Count res = 1;
    N /= P;
    while (!(N % P)) {
        ++res;
        N /= P;
    }
    return res;
}

// Factorize N into Prod{Pow(P[i], K[i])}
// Require: N > 0
// Return: Number of prime factors
template<class Unsigned, class Count>
constexpr ::std::size_t Factorize(Unsigned N, Unsigned P[], Count K[]) noexcept {
    ::std::size_t cnt = 0;
    if (!(N & 1)) {
        P[cnt] = 2;
        K[cnt++] = ImplFactorize<Unsigned, Count>(N, 2);
    }
    if (!(N % 3)) {
        P[cnt] = 3;
        K[cnt++] = ImplFactorize<Unsigned, Count>(N, 3);
    }
    for (Unsigned i = 5; i * i <= N; i += 6) {
        if (!(N % i)) {
            P[cnt] = i;
            K[cnt++] = ImplFactorize<Unsigned, Count>(N, i);
        }
        if (!(N % (i + 2))) {
            P[cnt] = i + 2;
            K[cnt++] = ImplFactorize<Unsigned, Count>(N, i + 2);
        }
    }
    if (N != 1) {
        P[cnt] = N;
        K[cnt++] = 1;
    }
    return cnt;
}

// Impl: Log2
template<class Unsigned, ::std::size_t = sizeof(Unsigned) * 8>
struct ImplLog2;

template<class Unsigned>
struct ImplLog2<Unsigned, 32> {
    alignas(32) static constexpr ::std::uint8_t Table[32]{
        0,  9,  1,  10, 13, 21, 2,  29,
        11, 14, 16, 18, 22, 25, 3,  30,
        8,  12, 20, 28, 15, 17, 24, 7,
        19, 27, 23, 6,  26, 5,  4,  31,
    };
    static constexpr int Log2(Unsigned N) noexcept {
        N |= N >> 1;
        N |= N >> 2;
        N |= N >> 4;
        N |= N >> 8;
        N |= N >> 16;
        return (int) Table[(N * 0x07c4acddu) >> 27];
    }
};

template<class Unsigned>
alignas(32) constexpr ::std::uint8_t ImplLog2<Unsigned, 32>::Table[32];

template<class Unsigned>
struct ImplLog2<Unsigned, 64> {
    alignas(64) static constexpr ::std::uint8_t Table[64]{
        0,  58, 1,  59, 47, 53, 2,  60, 39, 48, 27, 54, 33, 42, 3,  61,
        51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4,  62,
        57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21, 56,
        45, 25, 31, 35, 16, 9,  12, 44, 24, 15, 8,  23, 7,  6,  5,  63,
    };
    static constexpr int Log2(Unsigned N) noexcept {
        N |= N >> 1;
        N |= N >> 2;
        N |= N >> 4;
        N |= N >> 8;
        N |= N >> 16;
        N |= N >> 32;
        return (int) Table[(N * 0x03f6eaf2cd271461u) >> 58];
    }
};

template<class Unsigned>
alignas(64) constexpr ::std::uint8_t ImplLog2<Unsigned, 64>::Table[64];

// Return: Floor(Log2(N)) or 0 when N = 0
template<class Unsigned>
constexpr int Log2(Unsigned N) noexcept {
    return ImplLog2<Unsigned>::Log2(N);
}

// Return: Ceil(Log2(N)) or 0 when N = 0
template<class Unsigned>
constexpr int Log2Ceil(Unsigned N) noexcept {
    return Log2((N << 1) - 1);
}
