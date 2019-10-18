// Greatest Common Divisor
#pragma once

#include "Integer.hpp"

// Return: GCD of a and b
template<class UInt>
constexpr UInt Gcd(UInt a, UInt b) noexcept {
  while (b) {
    auto c = a % b;
    a = b;
    b = c;
  }
  return a;
};

// Impl: Result of GcdEx
template<class UInt_, class SInt_ = MakeSInt<UInt_>>
struct GcdRes {
  using UInt = UInt_;
  using SInt = SInt_;
  UInt d;
  SInt x;
  SInt y;
};

// Solve A * x + B * y = d; where d is Gcd(A, B)
// Return: {d, x, y}
template<class UInt, class SInt = MakeSInt<UInt_>>
constexpr GcdRes<UInt, SInt> GcdEx(const UInt& a, const SInt& b) noexcept {
  if (!b)
    return {a, 1, 0};
  auto res = GcdEx<UInt, SInt>(b, a % b);
  return {res.d, res.y, res.x - res.y * (SInt) (a / b)};
}
