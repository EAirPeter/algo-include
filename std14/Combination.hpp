// Combination, Factorial, Lucas's Theorem
#pragma once

#include <algorithm>

#include "Crt.hpp"
#include "ModArith.hpp"
#include "Utility.hpp"

// Generate factorial table for all 0 <= i <= N
// Require: No overflow
template<class Unsigned>
constexpr void GenFacTable(Unsigned Fac[], Unsigned N) noexcept {
    Fac[0] = 1;
    for (Unsigned i = 1; i <= N; ++i)
        Fac[i] = i * Fac[i - 1];
}

// Generate factorial table for all 0 <= i <= N, modulo Mod
template<class Unsigned, class ModRes = Unsigned>
constexpr void GenFacTable(Unsigned Fac[], Unsigned N, Unsigned Mod) noexcept {
    using MA = ModArith<Unsigned, ModRes>;
    Fac[0] = 1;
    for (Unsigned i = 1; i <= N; ++i)
        Fac[i] = MA::Mul(i, Fac[i - 1], Mod);
}

// Generate modular multiplicateive inverse table of factorials for all 0 <= i <= N, modulo Mod
// Require: FacN = Fac(N), Gcd(FacN, Mod) = 1
template<class Unsigned, class ModRes = Unsigned>
constexpr void GenFacInvTable(Unsigned FacInv[], Unsigned N, Unsigned FacN, Unsigned Mod) noexcept {
    using MA = ModArith<Unsigned, ModRes>;
    FacInv[N] = MA::Inv(FacN, Mod);
    for (Unsigned i = N; i; --i)
        FacInv[i - 1] = MA::Mul(i, FacInv[i]);
}

// Combination: Straightforward
// Require: 0 <= N <= M
// Require: No overflow
// Return: Combination number C(M, N) = Fac(M) / (Fac(N) * Fac(M - N))
template<class Unsigned>
constexpr Unsigned Comb(Unsigned M, Unsigned N) noexcept {
    N = ::std::min(N, M - N);
    Unsigned num = 1, den = 1;
    for (Unsigned i = 0; i < N; ++i) {
        num *= M - i + 1;
        den *= i + 1;
    }
    return num / den;
}

// Combination: Straightforward with factorial table
// Require: 0 <= N <= M
// Require: factorial table Fac
// Require: No overflow
// Return: C(M, N)
template<class Unsigned>
constexpr Unsigned Comb(Unsigned M, Unsigned N, const Unsigned Fac[]) noexcept {
    return Fac[M] / (Fac[N] * Fac[M - N]);
}

// Combination Modulus: Straightforward
// Require: 0 <= N <= M; N < Mod
// Return: C(M, N) % Mod
template<class Unsigned, class MulRes = Unsigned>
constexpr Unsigned Comb(Unsigned M, Unsigned N, Unsigned Mod) noexcept {
    using MA = ModArith<Unsigned, MulRes>;
    N = ::std::min(N, M - N);
    auto m = MA::Norm(M, Mod);
    auto n = MA::Norm(N, Mod);
    Unsigned num = 1, den = 1;
    for (Unsigned i = 0; i < N; ++i) {
        num = MA::Mul(num, m, Mod);
        den = MA::Mul(den, n, Mod);
        m = MA::Sub(m, 1, Mod);
        n = MA::Sub(n, 1, Mod);
    }
    return MA::Div(num, den, Mod);
}

// Combination Modulus: Straightforward with factorial table
// Require: 0 <= N <= M; N < Mod
// Require: factorial table Fac, multiplicative inverse table of factorials FacInv
// Return: C(M, N) % Mod
template<class Unsigned, class MulRes = Unsigned>
constexpr Unsigned Comb(
    Unsigned M, Unsigned N, Unsigned Mod, const Unsigned Fac[], const Unsigned FacInv[]
) noexcept {
    using MA = ModArith<Unsigned, MulRes>;
    return MA::Mul(Fac[M], MA::Mul(FacInv[N], FacInv[M - N], Mod), Mod);
    return 0;
}

// Combination Modulo Prime: Lucas' Theorem
// C(M, N) \equiv C(M % Mod, N % Mod) * C(M / Mod, N / Mod)
// Require: Mod must be prime
// Return: C(M, N) % Mod
template<class Unsigned, class MulRes = Unsigned>
constexpr Unsigned CombLucas(Unsigned M, Unsigned N, Unsigned Mod) noexcept {
    using MA = ModArith<Unsigned, MulRes>;
    Unsigned res = 1;
    while (N >= Mod) {
        res = MA::Mul(res, Comb<Unsigned, MulRes>(M % Mod, N % Mod, Mod), Mod);
        M /= Mod;
        N /= Mod;
    }
    return MA::Mul(res, Comb<Unsigned, MulRes>(M, N, Mod), Mod);
}

// Combination Modulo Prime: Lucas' Theorem with factorial table
// Require: Mod must be prime
// Require: factorial table Fac, multiplicative inverse table of factorials FacInv
// Return: C(M, N) % Mod
template<class Unsigned, class MulRes = Unsigned>
constexpr Unsigned CombLucas(
    Unsigned M, Unsigned N, Unsigned Mod, const Unsigned Fac[], const Unsigned FacInv[]
) noexcept {
    using MA = ModArith<Unsigned, MulRes>;
    Unsigned res = 1;
    while (N >= Mod) {
        res = MA::Mul(res, Comb<Unsigned, MulRes>(M % Mod, N % Mod, Mod, Fac, FacInv), Mod);
        M /= Mod;
        N /= Mod;
    }
    return MA::Mul(res, Comb<Unsigned, MulRes>(M, N, Mod), Mod, Fac, FacInv);
}

// Impl: Result of FacEx
template<class Unsigned_>
struct FacExRes {
    using Unsigned = Unsigned_;
    Unsigned rem;
    Unsigned exp;
};

// Solve rem, exp for: Fac(N) / Pow(P, exp) \equiv rem (mod Mod)
// where Gcd(Fac(N) / Pow(P, exp), P) = 1 and 1 <= rem < Mod
// Require: P must be prime, Mod must be power of P
// Return: {rem, exp}
template<class Unsigned, class MulRes = Unsigned>
constexpr FacExRes<Unsigned> FacEx(Unsigned N, Unsigned Mod, Unsigned P) noexcept {
    using MA = ModArith<Unsigned, MulRes>;
    Unsigned rem = 1, exp = 0, tmp = 1;
    if (N >= Mod)
        for (Unsigned i = 0; i < Mod; i += P)
            for (Unsigned j = i + 1; j < i + P; ++j)
                tmp = MA::Mul(tmp, j, Mod);
    while (N) {
        rem = MA::Mul(rem, MA::Pow(tmp, N / Mod, Mod), Mod);
        auto base = N / Mod * Mod;
        for (Unsigned i = base; i + P <= N; i += P)
            for (Unsigned j = i + 1; j < i + P; ++j)
                rem = MA::Mul(rem, j - base, Mod);
        for (Unsigned i = N / P * P + 1; i <= N; ++i)
            rem = MA::Mul(rem, i - base, Mod);
        exp += N / P;
        N /= P;
    }
    return {rem, exp};
}

// Combination Modulo Prime Power: Generalized Lucas' Theorem
// Require: 0 <= N <= M
// Require: P must be prime, Mod must be power of P
template<class Unsigned, class MulRes = Unsigned>
constexpr Unsigned CombLucasEx(Unsigned M, Unsigned N, Unsigned Mod, Unsigned P) noexcept {
    using MA = ModArith<Unsigned, MulRes>;
    auto A = FacEx<Unsigned, MulRes>(M, Mod, P);
    auto B = FacEx<Unsigned, MulRes>(N, Mod, P);
    auto C = FacEx<Unsigned, MulRes>(M - N, Mod, P);
    return MA::Mul(MA::Div(A.rem, MA::Mul(B.rem, C.rem, Mod), Mod), MA::Pow(P, A.exp - B.exp - C.exp, Mod), Mod);
}

// Combination Modulus: Generalized Lucas' Theorem & Chinese Remainder Theorem
// Require: 0 <= N <= M
template<class Unsigned, class MulRes = Unsigned>
constexpr Unsigned CombLucasEx(Unsigned M, Unsigned N, Unsigned Mod) noexcept {
    Unsigned P[sizeof(Unsigned) * 8];
    Unsigned K[sizeof(Unsigned) * 8];
    auto CPri = Factorize(Mod, P, K);
    for (::std::size_t i = 0; i < CPri; ++i) {
        auto mod = Pow(P[i], K[i]);
        K[i] = CombLucasEx<Unsigned, MulRes>(M, N, mod, P[i]);
        P[i] = mod;
    }
    return Crt<Unsigned, MulRes>(K, P, CPri);
}
