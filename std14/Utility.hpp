// Miscellaneous Utilities
#pragma once

#include "Common.hpp"

// Return: Absolute value of signed value N
template<class Int>
constexpr ::std::make_unsigned_t<Int> Abs(Int N) noexcept {
  using UInt = ::std::make_unsigned_t<Int>;
  using SInt = ::std::make_signed_t<Int>;
  return  (SInt) N < 0 ? (UInt) -(SInt) N : (UInt) N;
}

// Require: No overflow
// Return: B-th power of A
template<class Arith, class UInt>
constexpr Arith Pow(Arith A, UInt B) noexcept {
  Arith res = 1;
  while (B) {
    if (B & 1)
      res *= A;
    A *= A;
    B >>= 1;
  }
  return res;
}

// Impl: Extract all factor P from N
template<class UInt, class Count>
constexpr Count ImplFactorize(UInt& N, UInt P) noexcept {
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
template<class UInt, class Count>
constexpr Uz Factorize(UInt P[], Count K[], UInt N) noexcept {
  Uz cnt = 0;
  if (!(N & 1)) {
    P[cnt] = 2;
    K[cnt++] = ImplFactorize<UInt, Count>(N, 2);
  }
  if (!(N % 3)) {
    P[cnt] = 3;
    K[cnt++] = ImplFactorize<UInt, Count>(N, 3);
  }
  for (UInt i = 5; i * i <= N; i += 6) {
    if (!(N % i)) {
      P[cnt] = i;
      K[cnt++] = ImplFactorize<UInt, Count>(N, i);
    }
    if (!(N % (i + 2))) {
      P[cnt] = i + 2;
      K[cnt++] = ImplFactorize<UInt, Count>(N, i + 2);
    }
  }
  if (N != 1) {
    P[cnt] = N;
    K[cnt++] = 1;
  }
  return cnt;
}

constexpr int Log2P1(unsigned int N) noexcept {
  return ::std::numeric_limits<unsigned int>::digits - __builtin_clz(N);
}

constexpr int Log2P1(unsigned long N) noexcept {
  return ::std::numeric_limits<unsigned long>::digits - __builtin_clzl(N);
}

constexpr int Log2P1(unsigned long long N) noexcept {
  return ::std::numeric_limits<unsigned long long>::digits -
    __builtin_clzll(N);
}

template<class UInt>
constexpr int Log2C(UInt N) noexcept { return N > 1 ? Log2P1(N - 1) : 0; }
