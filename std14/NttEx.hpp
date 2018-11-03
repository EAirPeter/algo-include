// Number Theoretic Transform - Arbitrary Mod
#pragma once

#include <cstdint>

#include "ModArith.hpp"
#include "Ntt.hpp"

// Number Theoretic Transform - Arbitrary Mod
// Combine answer of three individual transforms using CRT
// Note: The CRT implementation is a modified version
// Require: Mod1 * Mod2 * Mod3 must be greater than Pow(Mod - 1, 2) * Len
template<
    ::std::size_t MaxExp_,
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
    Ntt<MaxExp, Unsigned, Mod1, G1, MulRes1> T1;
    Ntt<MaxExp, Unsigned, Mod2, G2, MulRes1> T2;
    Ntt<MaxExp, Unsigned, Mod3, G3, MulRes1> T3;
    // Only for static initialization
    // Use placement new before performing any transform
    constexpr NttEx() noexcept = default;
    // Initialize the 3 Ntt instances
    constexpr NttEx(::std::size_t exp) noexcept : T1(exp), T2(exp), T3(exp) {}
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
