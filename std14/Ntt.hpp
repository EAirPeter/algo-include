// Number Theoretic Transform
#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

#include "ModArith.hpp"

// Number Theoretic Transform
// Require: Mod must be a proth prime
// Require: G must be a primitive root of Mod
// Note: When considering only subscript in range [A.Len + B.Len - Len - 1, Len),
//   circular convolution is equivalent to linear convolution
//   where Len is the length of the transform
template<
    int MaxExp_,
    class Unsigned_ = ::std::uint32_t,
    class MulRes_ = ::uint64_t,
    Unsigned_ Mod_ = 998244353,
    Unsigned_ G_ = 3
>
struct Ntt {
    static constexpr auto MaxExp = MaxExp_;
    static constexpr auto MaxLen = ::std::size_t{1} << MaxExp;
    using Unsigned = Unsigned_;
    using MulRes = MulRes_;
    using MA = ModArith<Unsigned, MulRes>;
    static constexpr auto Mod = Mod_;
    static constexpr auto G = G_;
    static constexpr auto GInv = MA::Inv(G, Mod);
    Unsigned WbFwd[MaxExp + 1];
    Unsigned WbInv[MaxExp + 1];
    Unsigned LenInv[MaxExp + 1];
    // Initialize for all 0 <= Exp <= MaxExp
    constexpr void InitAll(int MaxExp) noexcept {
        for (auto i = 0; i <= MaxExp; ++i)
            Init(i);
    }
    // Generate w table and its modular multiplicative inverse table
    // Generate LenInv
    constexpr void Init(int Exp) noexcept {
        WbFwd[Exp] = MA::Pow(G, (Mod - 1) >> Exp, Mod);
        WbInv[Exp] = MA::Pow(GInv, (Mod - 1) >> Exp, Mod);
        LenInv[Exp] = MA::Inv(MA::Norm(::std::size_t{1} << Exp, Mod), Mod);
    }
    // Perform NTT on sequence A
    // Require: The length of A must be Pow(2, Exp)
    // Require: 0 <= A[i] < Mod for all 0 <= i < Len
    constexpr void Forward(Unsigned A[], int Exp) const noexcept {
        XTransform(A, Exp, WbFwd[Exp]);
    }
    // Perform INTT on sequence A
    // Require: The length of A must be Pow(2, Exp)
    // Require: 0 <= A[i] < Mod for all 0 <= i < Len
    constexpr void Inverse(Unsigned A[], int Exp) const noexcept {
        XTransform(A, Exp, WbInv[Exp]);
        auto Len = ::std::size_t{1} << Exp;
        for (::std::size_t i = 0; i < Len; ++i)
            A[i] = MA::Mul(A[i], LenInv[Exp], Mod);
    }
    static constexpr void XTransform(Unsigned A[], int Exp, Unsigned Wb) noexcept {
        auto Len = ::std::size_t{1} << Exp;
        using ::std::swap;
        for (::std::size_t i = 0, j = 0; i < Len; ++i) {
            if (i > j)
                swap(A[i], A[j]);
            for (::std::size_t k = Len >> 1; (j ^= k) < k; k >>= 1);
        }
        Unsigned Wns[MaxExp];
        Wns[0] = Wb;
        for (auto i = 1; i < Exp; ++i)
            Wns[i] = MA::Mul(Wns[i - 1], Wns[i - 1], Mod);
        for (auto i = 0; i < Exp; ++i) {
            auto ChkSiz = ::std::size_t{1} << i;
            auto Wn = Wns[Exp - 1 - i];
            for (::std::size_t j = 0; j < Len; j += 2u << i) {
                Unsigned W = 1;
                for (::std::size_t k = j; k < j + ChkSiz; ++k) {
                    auto t = MA::Mul(W, A[k + ChkSiz], Mod);
                    A[k + ChkSiz] = MA::Sub(A[k], t, Mod);
                    A[k] = MA::Add(A[k], t, Mod);
                    W = MA::Mul(W, Wn, Mod);
                }
            }
        }
    }
};

// Number Theoretic Transform - Arbitrary Mod
// Combine answer of three individual transforms using CRT
// Note: The CRT implementation is a modified version
// Require: Mod1 * Mod2 * Mod3 must be greater than Pow(Mod - 1, 2) * Len
template<
    int MaxExp_,
    class Unsigned_ = ::std::uint32_t,
    class MulRes1_ = ::std::uint64_t,
    class MulRes2_ = MulResFP,
    Unsigned_ Mod1_ = 469762049,
    Unsigned_ G1_ = 3,
    Unsigned_ Mod2_ = 998244353,
    Unsigned_ G2_ = 3,
    Unsigned_ Mod3_ = 1004535809,
    Unsigned_ G3_ = 3
>
struct NttEx {
    static constexpr auto MaxExp = MaxExp_;
    using Unsigned = Unsigned_;
    using MulRes1 = MulRes1_;
    using MulRes2 = MulRes2_;
    static constexpr auto Mod1 = Mod1_;
    static constexpr auto G1 = G1_;
    static constexpr auto Mod2 = Mod2_;
    static constexpr auto G2 = G2_;
    static constexpr auto Mod3 = Mod3_;
    static constexpr auto G3 = G3_;
    using MA1 = ModArith<Unsigned, MulRes1>;
    using MA2 = ModArith<MulRes1, MulRes2>;
    static constexpr auto Mod4 = (MulRes1) Mod1 * Mod2;
    static constexpr auto Mod1Inv = MA1::Inv(Mod1, Mod2);
    static constexpr auto Mod2Inv = MA1::Inv(Mod2, Mod1);
    static constexpr auto Mod4Inv = MA1::Inv(MA1::Norm(Mod4, Mod3), Mod3);
    using Ntt1 = Ntt<MaxExp, Unsigned, MulRes1, Mod1, G1>;
    using Ntt2 = Ntt<MaxExp, Unsigned, MulRes1, Mod2, G2>;
    using Ntt3 = Ntt<MaxExp, Unsigned, MulRes1, Mod3, G3>;
    // Combine first two answers using CRT
    static constexpr MulRes1 CombA(Unsigned A1, Unsigned A2) noexcept {
        return MA2::Add(
            MA2::Mul((MulRes1) A1 * Mod2, Mod2Inv, Mod4),
            MA2::Mul((MulRes1) A2 * Mod1, Mod1Inv, Mod4),
            Mod4
        );
    }
    // Combine the answer from CombA and the third answer using CRT
    static constexpr Unsigned CombB(Unsigned A3, MulRes1 A4, Unsigned Mod) noexcept {
        return MA1::Norm(
            A4 + (Mod4 % Mod) * MA1::Mul(A3 + Mod3 - MA1::Norm(A4, Mod3), Mod4Inv, Mod3),
            Mod
        );
    }
    // Combine three answers using CRT
    // Require: A_i must be in [0, Mod_i) for i in {1, 2, 3}
    static constexpr Unsigned Combine(Unsigned A1, Unsigned A2, Unsigned A3, Unsigned Mod) noexcept {
        return CombB(A3, CombA(A1, A2), Mod);
    }
};

// OEIS A080076: Proth Primes
// Primes of the form P = K * Pow(2, M) + 1 with odd K < Pow(2, M) and M >= 1
// G is a primitive root of P
// P                    K   M   G
// 3                    1   1   2
// 5                    1   2   2
// 17                   1   4   3
// 97                   3   5   5
// 193                  3   6   5
// 257                  1   8   3
// 7681                 15  9   17
// 12289                3   12  11
// 40961                5   13  3
// 65537                1   16  3
// 786433               3   18  10
// 5767169              11  19  3
// 7340033              7   20  3
// 23068673             11  21  3
// 104857601            25  22  3
// 167772161            5   25  3
// 469762049            7   26  3
// 998244353            119 23  3
// 1004535809           479 21  3
// 2013265921           15  27  31
// 2281701377           17  27  3
// 3221225473           3   30  5
// 75161927681          35  31  3
// 77309411329          9   33  7
// 206158430209         3   36  22
// 2061584302081        15  37  7
// 2748779069441        5   39  3
// 6597069766657        3   41  5
// 39582418599937       9   42  5
// 79164837199873       9   43  5
// 263882790666241      15  44  7
// 1231453023109121     35  45  3
// 1337006139375617     19  46  3
// 3799912185593857     27  47  5
// 4222124650659841     15  48  19
// 7881299347898369     7   50  6
// 31525197391593473    7   52  3
// 180143985094819841   5   55  6
// 1945555039024054273  27  56  5
// 4179340454199820289  29  57  3
