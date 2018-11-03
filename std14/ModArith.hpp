// Modular Arithmetics
#pragma once

#include <type_traits>

#include "Gcd.hpp"

// Use this as MulRes when calculating ModMul(A, B, Mod);
// where A and B are uint64_t and A * B is not representable by uint64_t
// Require: extended precision floating point number (sizeof(long double) >= 10)
struct MulResFP {};

// Impl: ModNorm of unsigned
template<class Unsigned, class Integral, bool = ::std::is_unsigned<Integral>()>
struct ImplModNorm {
    static constexpr Unsigned Norm(Integral N, Unsigned Mod) noexcept {
        return (Unsigned) (N % Mod);
    }
};

// Impl: ModNorm of signed integer
template<class Unsigned, class Integral>
struct ImplModNorm<Unsigned, Integral, false> {
    static constexpr Unsigned Norm(Integral N, Unsigned Mod) noexcept {
        using Signed = ::std::make_signed_t<Unsigned>;
        auto res = N % (Signed) Mod;
        return (Unsigned) (res < 0 ? res + (Signed) Mod : res);
    }
};

// Impl: ModMul using normal multiplication
template<class Unsigned, class MulRes>
struct ImplModMul {
    static constexpr Unsigned Mul(Unsigned A, Unsigned B, Unsigned Mod) noexcept {
        return (Unsigned) ((MulRes) A * B % Mod);
    }
};

// Impl: ModMul using extended precision floating point number multiplication
template<class Unsigned>
struct ImplModMul<Unsigned, MulResFP> {
    static constexpr Unsigned Mul(Unsigned A, Unsigned B, Unsigned Mod) noexcept {
#   if defined(MA_USEI128) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 6))
        return (Unsigned) ((unsigned __int128) A * B % Mod);
#   else
        using Signed = ::std::make_signed_t<Unsigned>;
        return ImplModNorm<Unsigned, Signed>::Norm((Signed)
            (A * B - (Unsigned) ((long double) A / Mod * B + 1e-6) * Mod), Mod
        );
#   endif
    }
};

// Modular Arithmetic
// All % operator in comment means mathematical modular operation
// Require: Mod > 0
template<class Unsigned_, class MulRes_ = Unsigned_>
struct ModArith {
    using Unsigned = Unsigned_;
    using MulRes = MulRes_;
    // Return: N % Mod
    template<class Integral>
    static constexpr Unsigned Norm(Integral N, Unsigned Mod) noexcept {
        return ImplModNorm<Unsigned, Integral>::Norm(N, Mod);
    }
    // Require: 0 <= A, B < Mod
    // Return: (A + B) % Mod
    static constexpr Unsigned Add(Unsigned A, Unsigned B, Unsigned Mod) noexcept {
        auto res = A + B;
        return res < Mod ? res : res - Mod;
    }
    // Require: 0 <= A, B < Mod
    // Return: (A - B) % Mod
    static constexpr Unsigned Sub(Unsigned A, Unsigned B, Unsigned Mod) noexcept {
        auto res = A - B;
        return A < B ? res + Mod : res;
    }
    // Return: (A * B) % Mod
    static constexpr Unsigned Mul(Unsigned A, Unsigned B, Unsigned Mod) noexcept {
        return ImplModMul<Unsigned, MulRes>::Mul(A, B, Mod);
    }
    // Return: Pow(A, B) % Mod
    static constexpr Unsigned Pow(Unsigned A, Unsigned B, Unsigned Mod) noexcept {
        Unsigned res = 1;
        while (B) {
            if (B & 1)
                res = Mul(res, A, Mod);
            A = Mul(A, A, Mod);
            B >>= 1;
        }
        return res;
    }
    // Require: Gcd(N, Mod) = 1
    // Return: Multiplicative inverse of an N with respect to Mod, or (1 / B) % Mod
    static constexpr Unsigned Inv(Unsigned N, Unsigned Mod) noexcept {
        return Norm(GcdEx(N, Mod).x, Mod);
        // Pow(N, Mod - 2) => Mod must be prime
        // Pow(N, PrimeTable::Phi[Mod] - 1) => for arbitrary Mod 
        // InversionTable::Inv[N] => Mod must be prime
    }
    // Require: Gcd(B, Mod) = 1
    // Return: (A / B) % Mod
    static constexpr Unsigned Div(Unsigned A, Unsigned B, Unsigned Mod) noexcept {
        return Mul(A, Inv(B, Mod), Mod);
    }
    // Generate multiplicative inverses for all 1 <= i <= N < Mod with respect to Mod
    // Require: Mod must be prime
    static constexpr void GenInvTable(Unsigned Inv[], Unsigned N, Unsigned Mod) noexcept {
        Inv[1] = 1;
        for (Unsigned i = 2; i <= N; ++i)
            Inv[i] = Mul(Mod - Mod / i, Inv[Mod % i], Mod);
    }
};
