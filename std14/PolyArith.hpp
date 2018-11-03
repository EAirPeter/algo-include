// Polynomial Arithmetic
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include "Ntt.hpp"
#include "Utility.hpp"

template<class Ntt_>
struct PolyArith {
    using Ntt = Ntt_;
    static constexpr auto MaxExp = Ntt::MaxExp;
    static constexpr auto MaxLen = Ntt::MaxLen;
    using MA = typename Ntt::MA;
    using Unsigned = typename Ntt::Unsigned;
    using MulRes = typename Ntt::MulRes;
    static constexpr auto Mod = Ntt::Mod;
    using TmpBuf = Unsigned[MaxLen];
    Ntt& NTT;
    TmpBuf TA, TB, TC;
    TmpBuf FNtt, FRInvNtt;
    ::std::size_t N, M;
    inline PolyArith(Ntt& NTT_) : NTT(NTT_) {}
    // Inversion
    // Require: T != A
    constexpr void Inv(Unsigned T[], const Unsigned A[], ::std::size_t N) noexcept {
        if (N == 1) {
            T[0] = MA::Inv(A[0], Mod);
            return;
        }
        auto M = (N + 1) >> 1;
        Inv(T, A, M);
        auto Exp = Log2Ceil(N + M - 1);
        auto Len = ::std::size_t{1} << Exp;
        ::std::copy(A, A + N, TA);
        ::std::fill(TA + N, TA + Len, 0);
        NTT.Forward(TA, Exp);
        ::std::copy(T, T + M, TB);
        ::std::fill(TB + M, TB + Len, 0);
        NTT.Forward(TB, Exp);
        for (::std::size_t i = 0; i < Len; ++i)
            TB[i] = MA::Mul(MA::Sub(2, MA::Mul(TA[i], TB[i], Mod), Mod), TB[i], Mod);
        NTT.Inverse(TB, Exp);
        ::std::copy(TB + M, TB + N, T + M);
    }
    // Division: A \equiv F * G + H (mod x^N)
    //           N        M   R   M-1
    // N = M + R - 1 => R = N - M + 1
    // A is dividend; F is divisor; G is quotient; H is remainder
    // AR \equiv FR * GR + x^R * HR (mod x^N)
    // AR \equiv FR * GR (mod x^R)
    // GR \equiv AR * Inv(FR) (mod x^R)
    // H \equiv A - F * G (mod x^N)
    // Set divisor for known N
    constexpr void SetDivisor(::std::size_t N_, const Unsigned F[], ::std::size_t M_) noexcept {
        N = N_;
        M = M_;
        auto R = N - M + 1;
        auto ExpN = Log2Ceil(N);
        auto ExpR = Log2Ceil(R + R - 1);
        ::std::copy(F, F + M, FNtt);
        ::std::fill(FNtt + M, FNtt + (1 << ExpN), 0);
        NTT.Forward(FNtt, ExpN);
        auto cnt = ::std::min(M, R);
        ::std::reverse_copy(F + M - cnt, F + M, TC);
        ::std::fill(TC + cnt, TC + R, 0);
        Inv(FRInvNtt, TC, R);
        ::std::fill(FRInvNtt + R, FRInvNtt + (1 << ExpR), 0);
        NTT.Forward(FRInvNtt, ExpR);
    }
    // Euclidean Division: Quotient
    constexpr void Div(Unsigned G[], const Unsigned A[]) noexcept {
        XDiv(A);
        auto R = N - M + 1;
        ::std::reverse_copy(TC, TC + R, G);
    }
    // Euclidean Division: Remainder
    constexpr void Rem(Unsigned H[], const Unsigned A[]) noexcept {
        XDiv(A);
        auto R = N - M + 1;
        ::std::reverse(TC, TC + R);
        XRem(H, A);
    }
    // Euclidean Division: Quotient and Remainder
    // Require: G != A
    constexpr void DivRem(Unsigned G[], Unsigned H[], const Unsigned A[]) noexcept {
        XDiv(A);
        auto R = N - M + 1;
        ::std::reverse(TC, TC + R);
        ::std::copy(TC, TC + R, G);
        XRem(H, A);
    }
    constexpr void XDiv(const Unsigned A[]) noexcept {
        auto R = N - M + 1;
        auto ExpR = Log2Ceil(R + R - 1);
        auto LenR = ::std::size_t{1} << ExpR;
        auto cnt = ::std::min(N, R);
        ::std::reverse_copy(A + N - cnt, A + N, TC); // var G is poly AR (mod x^R)
        ::std::fill(TC + cnt, TC + LenR, 0);
        NTT.Forward(TC, ExpR);
        for (::std::size_t i = 0; i < LenR; ++i)
            TC[i] = MA::Mul(TC[i], FRInvNtt[i], Mod);
        NTT.Inverse(TC, ExpR); // var G is poly GR (mod x^R)
    }
    constexpr void XRem(Unsigned H[], const Unsigned A[]) noexcept {
        auto R = N - M + 1;
        auto ExpN = Log2Ceil(N);
        auto LenN = ::std::size_t{1} << ExpN;
        ::std::fill(TC + R, TC + LenN, 0);
        NTT.Forward(TC, ExpN);
        for (::std::size_t i = 0; i < LenN; ++i)
            TC[i] = MA::Mul(TC[i], FNtt[i], Mod);
        NTT.Inverse(TC, ExpN);
        for (::std::size_t i = 0; i < M - 1; ++i)
            H[i] = MA::Sub(A[i], TC[i], Mod);
    }
    // Derivative
    // Len(T) = N - 1
    constexpr void Der(Unsigned T[], const Unsigned A[], ::std::size_t N) noexcept {
        for (::std::size_t i = 1; i < N; ++i)
            T[i - 1] = MA::Mul(A[i], (Unsigned) i, Mod);
    }
    // Integral
    // Len(T) = N + 1
    constexpr void Int(Unsigned T[], const Unsigned A[], ::std::size_t N) noexcept {
        for (::std::size_t i = N; i; --i)
            T[i] = MA::Div(A[i - 1], (Unsigned) i, Mod);
        T[0] = 0;
    }
    // Logarithm
    constexpr void Log(Unsigned T[], const Unsigned A[], ::std::size_t N) noexcept {
        Inv(TC, A, N);
        auto Exp = Log2Ceil(N + N - 1);
        auto Len = ::std::size_t{1} << Exp;
        ::std::fill(TC + N, TC + Len, 0);
        NTT.Forward(TC, Exp);
        Der(TB, A, N);
        ::std::fill(TB + N - 1, TB + Len, 0);
        NTT.Forward(TB, Exp);
        for (::std::size_t i = 0; i < Len; ++i)
            TB[i] = MA::Mul(TB[i], TC[i], Mod);
        NTT.Inverse(TB, Exp);
        Int(T, TB, N - 1);
    }
    // Newton's Method
    // Solve F(A) \equiv 0 (mod x^N)
    // F \equiv A0 - F(A0) * Inv(F'(A0)) (mod x^N)
    //   where F(A0) \equiv 0 (mod x^((N + 1)/ 2))
    // Exponent
    // F = Log(T) - A
    // T \equiv T0 * (1 - Log(T0) + A) (mod x^N)
    // Require: T != A
    // Require: T must be large enough to perform NTT
    constexpr void Exp(Unsigned T[], const Unsigned A[], ::std::size_t N) noexcept {
        if (N == 1) {
            T[0] = 1;
            return;
        }
        auto M = (N + 1) >> 1;
        Exp(T, A, M);
        auto Exp = Log2Ceil(N + M - 1);
        auto Len = ::std::size_t{1} << Exp;
        ::std::fill(T + M, T + Len, 0);
        Log(TC, T, N);
        TC[0] = MA::Sub(TC[0], 1, Mod);
        for (::std::size_t i = 0; i < N; ++i)
            TC[i] = MA::Sub(A[i], TC[i], Mod);
        ::std::fill(TC + N, TC + Len, 0);
        NTT.Forward(TC, Exp);
        NTT.Forward(T, Exp);
        for (::std::size_t i = 0; i < Len; ++i)
            T[i] = MA::Mul(T[i], TC[i], Mod);
        NTT.Inverse(T, Exp);
    }
};
