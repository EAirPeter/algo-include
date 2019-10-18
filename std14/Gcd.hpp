// Greatest Common Divisor
#pragma once

#include "Common.hpp"

// Return: GCD of a and b
template<class UInt>
constexpr UInt Gcd(UInt A, UInt B) noexcept {
  while (B) { auto C = A % B; A = B; B = C; }
  return A;
}

// Impl: Result of GcdEx
template<class UInt_, class SInt_ = ::std::make_signed_t<UInt_>>
struct GcdRes {
  using UInt = UInt_;
  using SInt = SInt_;
  UInt d;
  SInt x;
  SInt y;
};

// Solve A * x + B * y = d; where d is Gcd(A, B)
// Return: {d, x, y}
template<class UInt, class SInt = ::std::make_signed_t<UInt>>
constexpr GcdRes<UInt, SInt> GcdEx(UInt A, UInt B) noexcept {
  if (!B) return {A, 1, 0};
  auto res = GcdEx<UInt, SInt>(B, A % B);
  return {res.d, res.y, res.x - res.y * (SInt) (A / B)};
}
