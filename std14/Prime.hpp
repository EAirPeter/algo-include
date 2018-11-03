// Prime Number, Euler's Totient Function, Mobius Function, Integer Factorization
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "ModArith.hpp"
#include "Utility.hpp"

// Generate prime table for all 1 <= i <= N
// Require: Vis must be zero
// Return: Number of primes
template<class Unsigned, class MulRes = Unsigned>
constexpr ::std::size_t GenPrimeTable(Unsigned Pri[], bool Vis[], Unsigned N) noexcept {
    ::std::size_t cnt = 0;
    Vis[1] = true;
    for (Unsigned i = 2; i <= N; ++i) {
        if (!Vis[i])
            Pri[cnt++] = i;
        for (::std::size_t j = 0; j < cnt && (MulRes) i * Pri[j] <= N; ++j) {
            Vis[i * Pri[j]] = true;
            if (!(i % Pri[j]))
                break;
        }
    }
    return cnt;
}

// Generate prime table and phi table for all 1 <= i <= N
// Require: Pri must contain all prime numbers in [1, N]
// Return: Number of primes
template<class Unsigned, class MulRes = Unsigned>
constexpr void GenPhiTable(Unsigned Phi[], Unsigned N, const Unsigned Pri[], ::std::size_t PriCnt) noexcept {
    ::std::size_t cnt = 0;
    Phi[1] = 1;
    for (Unsigned i = 2; i <= N; ++i) {
        if (Pri[cnt] == i) {
            Phi[i] = i - 1;
            ++cnt;
        }
        for (::std::size_t j = 0; j < cnt && (MulRes) i * Pri[j] <= N; ++j) {
            if (i % Pri[j])
                Phi[i * Pri[j]] = Phi[i] * (Pri[j] - 1);
            else {
                Phi[i * Pri[j]] = Phi[i] * Pri[j];
                break;
            }
        }
    }
}

// Generate prime table and mu table for all 1 <= i <= N
// Require: Vis must be zero
// Return: Number of primes
template<class Unsigned, class Signed = ::std::make_signed_t<Unsigned>, class MulRes = Unsigned>
constexpr void GenMuTable(Signed Mu[], Unsigned N, const Unsigned Pri[], ::std::size_t PriCnt) noexcept {
    ::std::size_t cnt = 0;
    Mu[1] = 1;
    for (Unsigned i = 2; i <= N; ++i) {
        if (Pri[cnt] == i) {
            Mu[i] = -1;
            ++cnt;
        }
        for (::std::size_t j = 0; j < cnt && (MulRes) i * Pri[j] <= N; ++j) {
            Vis[i * Pri[j]] = true;
            if (i % Pri[j])
                Mu[i * Pri[j]] = 0;
            else
                Mu[i * Pri[j]] = -Mu[i];
        }
    }
}

// Primality Test: Optimized Brute Force
// Prime number must be 6 * k - 1 or 6 * k + 1, except for 2 and 3
// Require: N > 0
// Return: Primality of N
template<class Unsigned>
constexpr bool IsPrime6(Unsigned N) noexcept {
    if (N == 2 || N == 3)
        return true;
    if (N < 2 || !(N & 1) || !(N % 3))
        return false;
    for (Unsigned i = 5; i * i <= N; i += 6)
        if (!(N % i) || !(N % (i + 2)))
            return false;
    return true;
}

// Primality Test: Miller-Rabin
// N = Pow(2, R) * D + 1; D is odd
template<class Unsigned, class MulRes, ::std::uintmax_t... Bases>
struct MillerRabin {
    // Require: N > 0
    // Return: Primality of N
    static constexpr bool IsPrime(Unsigned N) noexcept {
        if (N == 2 || N == 3)
            return true;
        if (N < 2 || !(N & 1) || !(N % 3))
            return false;
        auto D = N - 1;
        while (!(D & 1))
            D >>= 1;
        using MA = ModArith<Unsigned, MulRes>;
        constexpr ::std::uintmax_t bases[]{Bases...};
        for (auto base : bases) {
            auto x = (Unsigned) (base % N);
            if (!x)
                continue;
            x = MA::Pow(x, D, N);
            if (x == 1 || x == N - 1)
                continue;
            for (auto d = D << 1; d != N - 1; d <<= 1) {
                x = MA::Mul(x, x, N);
                if (x == 1)
                    return false;
                if (x == N - 1)
                    break;
            }
            if (x != N - 1)
                return false;
        }
        return true;
    }
};

constexpr ::std::uint32_t Mr1Bound = 341531u;
using Mr1 = MillerRabin<::std::uint32_t, ::std::uint64_t, 9'345883'071009'581737u>;
constexpr ::std::uint32_t Mr2Bound = 1050'535501u;
using Mr2 = MillerRabin<::std::uint32_t, ::std::uint64_t, 336781'006125u, 9639'812373'923155u>;
constexpr ::std::uint64_t Mr3Bound = 350269'456337u;
using Mr3 = MillerRabin<::std::uint64_t, MulResFP, 4'230279'247111'683200u,
      14'694767'155120'705706u, 16'641139'526367'750375u>;
constexpr ::std::uint64_t Mr4Bound = 55'245642'489451u;
using Mr4 = MillerRabin<::std::uint64_t, MulResFP, 2u, 141'889084'524735u,
      1'199'124'725'622'454'117u, 11'096072'698276'303650u>;
constexpr ::std::uint64_t Mr5Bound = 7999'252175'582851u;
using Mr5 = MillerRabin<::std::uint64_t, MulResFP, 2u, 4'130806'001517u,
      149795'463772'692060u, 186635'894390'467037u, 3'967304'179347'715805u>;
constexpr ::std::uint64_t Mr6Bound = 585226'005592'931977u;
using Mr6 = MillerRabin<::std::uint64_t, MulResFP, 2u, 123'635709'730000u,
      9233'062284'813009u, 43835'965440'333360u, 761179'012939'631437u, 1'263739'024124'850375u>;
using MrU32 = MillerRabin<::std::uint32_t, ::std::uint64_t, 2u, 7u, 61u>;
using MrU64 = MillerRabin<::std::uint64_t, MulResFP, 2u, 325u, 9375u, 28178u, 450775u, 9'780504u, 1795'265022u>;

// Primality Test: Adaptive Miller-Rabin
// Use the smallest MillerRabin on the given N
// Require: N > 0
// Return: Primality of N
template<class Unsigned, class MulRes = Unsigned>
constexpr bool IsPrimeMr(Unsigned N) noexcept {
    return N < Mr1Bound ? Mr1::IsPrime((::std::uint32_t) N) :
        N < Mr2Bound ? Mr2::IsPrime((::std::uint32_t) N) :
        N < Mr3Bound ? Mr3::IsPrime((::std::uint64_t) N) :
        N < Mr4Bound ? Mr4::IsPrime((::std::uint64_t) N) :
        N < Mr5Bound ? Mr5::IsPrime((::std::uint64_t) N) :
        N < Mr6Bound ? Mr6::IsPrime((::std::uint64_t) N) :
        MrU64::IsPrime((::std::uint64_t) N);
}

// Pollard's Rho Algorithm: Monte Carlo
// Use polynomial: f(x) = (x * x + a) % N
// Require: N must not be prime
// Require: N > 1
// Return: one factor d of N; where Gcd(N / d, d) = 1 and 1 < d
//   d can be either composite or prime
template<class Unsigned, class MulRes = Unsigned>
constexpr Unsigned PollardRho(Unsigned N, Unsigned x, Unsigned a) noexcept {
    using MA = ModArith<Unsigned, MulRes>;
    auto f = [N, a](Unsigned y) { return MA::Add(MA::Mul(y, y, N), a, N); };
    constexpr Unsigned m = (Unsigned) (sizeof(Unsigned) * sizeof(Unsigned) * sizeof(Unsigned));
    Unsigned y = x, ys, q = 1, G = 1;
    for (Unsigned r = 1; G == 1; r <<= 1) {
        x = y;
        for (Unsigned i = 0; i < r; ++i)
            y = f(y);
        for (Unsigned k = 0; k < r && G == 1; k += m) {
            ys = y;
            for (Unsigned i = 0; i < ::std::min(m, r - k); ++i) {
                y = f(y);
                q = MA::Mul(q, Abs(x - y), N);
            }
            G = Gcd(q, N);
            k += m;
        }
    }
    if (G == N) {
        do {
            ys = f(ys);
            G = Gcd(Abs(x - ys), N);
        } while (G == 1);
    }
    return G;
}

// Pollard's Rho Algorithm: Las Vegas
// Require: N must not be prime
// Require: N > 1
// Return: one factor d of N; where Gcd(N / d, d) = 1 and 1 < d < N
//   d can be either composite or prime
template<class Unsigned, class MulRes = Unsigned, class Rng>
constexpr Unsigned PollardRho(Unsigned N, Rng&& Rand) noexcept {
    Unsigned d = N;
    while (d == N)
        d = PollardRho<Unsigned, MulRes>(N, Rand() % N, Rand() % N);
    return d;
}
