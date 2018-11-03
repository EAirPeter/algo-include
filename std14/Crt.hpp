// Chinese Remainder Theorem
#pragma once

#include "ModArith.hpp"

// Chinese Remainder Theorem: Solve x \equiv A[i] (mod M[i])
// Require: Gcd(M[i], M[j]) = 1 for every i != j
// Require: 0 <= A[i] < M[i]
// Return: x; where 0 <= x < Prod{M[i]}
template<class Unsigned, class MulRes = Unsigned>
constexpr Unsigned Crt(const Unsigned A[], const Unsigned M[], ::std::size_t N) noexcept {
    using MA = ModArith<Unsigned, MulRes>;
    auto Mod = M[0];
    for (::std::size_t i = 1; i < N; ++i)
        Mod *= M[i];
    Unsigned res = 0;
    for (::std::size_t i = 0; i < N; ++i) {
        auto Mi = Mod / M[i];
        res = MA::Add(res, MA::Mul(MA::Mul(A[i], Mi, Mod), MA::Inv(Mi, M[i]), Mod), Mod);
    }
    return res;
}

// Impl: Result of MergeLce
// Linear Congruence Equation: x \equiv a (mod m)
template<class Unsigned_>
struct LceRes {
    using Unsigned = Unsigned_;
    Unsigned a;
    Unsigned m;
};

// Merge two LCE: x \equiv A (mod M) and x \equiv B (mod N)
// x = u * M + A = v * N + B
// u = ((B - A) / d) * Inv(M / d, N / d); where d = Gcd(M, N)
// x \equiv u * M + A (mod lcm); where lcm = Lcm(M, N) = N / d * M
// Require: B - A \equiv 0 (mod Gcd(M, N))
// Return: The merged LCE
template<class Unsigned, class MulRes = Unsigned>
constexpr LceRes<Unsigned> MergeLce(Unsigned A, Unsigned M, Unsigned B, Unsigned N) noexcept {
    using Signed = ::std::make_signed_t<Unsigned>;
    using MA = ModArith<Unsigned, MulRes>;
    auto gcd = Gcd(M, N);
    N /= gcd;
    auto lcm = M * N;
    auto u = MA::Mul(MA::Norm((Signed) (B - A) / (Signed) gcd, lcm), MA::Inv(MA::Norm(M / gcd, N), N), lcm);
    return {MA::Add(MA::Mul(u, M, lcm), A, lcm), lcm};
}

// Generalized Chinese Remainder Theorem: Solve x \equiv A[i] (mod M[i])
// Require: 0 <= A[i] < M[i]
// Return: x; where 0 <= x < Lcm{M[i]}
template<class Unsigned, class MulRes = Unsigned>
constexpr Unsigned CrtEx(const Unsigned A[], const Unsigned M[], ::std::size_t N) noexcept {
    LceRes<Unsigned> res{A[0], M[0]};
    for (::std::size_t i = 1; i < N; ++i)
        res = MergeLce<Unsigned, MulRes>(res.a, res.m, A[i], M[i]);
    return res.a;
}
