// Modular Arithmetics
// Use MulResFP if want 64b * 64 % 64b, define MA_USEI128 to use GCC __int128
#pragma once

#include "Gcd.hpp"

// Use this as MulRes when calculating ModMul(A, B, Mod);
// where A and B are uint64_t and A * B is not representable by uint64_t
// Require: extended precision floating point number
struct MulResFP {};

namespace ImplMA {
// Impl: ModNrm of signed integer
template<class UInt, class SInt_>
constexpr UInt NrmS(SInt_ N, UInt Mod) noexcept {
  auto res = N % (SInt_) Mod;
  return (UInt) (res < 0 ? res + (SInt_) Mod : res);
}

// Impl: ModNrm of unsigned
template<class UInt, class UInt_>
constexpr UInt NrmU(UInt_ N, UInt Mod) noexcept {
  return (UInt) (N % Mod);
}

// Impl: ModMul using normal multiplication
template<class UInt, class MulRes>
struct Mul_ {
  static constexpr UInt Mul(UInt A, UInt B, UInt Mod) noexcept {
    return NrmU((MulRes) A * B, Mod);
  }
};

// Impl: ModMul using extended precision floating point number multiplication
template<class UInt>
struct Mul_<UInt, MulResFP> {
  static constexpr UInt Mul(UInt A, UInt B, UInt Mod) noexcept {
#   if defined(MA_USEI128)
    return (UInt) ((unsigned __int128) A * B % Mod);
#   else
    return NrmS((::std::make_signed_t<UInt>)
      (A * B - Mod * (UInt) ((long double) A / Mod * B)), Mod);
#   endif
  }
};
} // namespace ImplMA

// Modular Arithmetic
// All % operator in comment means mathematical modular operation
// Require: Mod > 0
template<class UInt_, class MulRes_ = UInt_>
struct ModArith {
  using UInt = UInt_;
  using MulRes = MulRes_;
  // Return: N % Mod
  template<class XSInt_>
  static constexpr UInt NrmS(XSInt_ N, UInt Mod) noexcept {
    return ImplMA::NrmS<UInt, XSInt_>(N, Mod);
  }
  // Return: N % Mod
  template<class XUInt_>
  static constexpr UInt NrmU(XUInt_ N, UInt Mod) noexcept {
    return ImplMA::NrmU<UInt, XUInt_>(N, Mod);
  }
  // Require: 0 <= A, B < Mod
  // Return: (A + B) % Mod
  static constexpr UInt Add(UInt A, UInt B, UInt Mod) noexcept {
    auto res = A + B;
    return res < Mod ? res : res - Mod;
  }
  // Require: 0 <= A, B < Mod
  // Return: (A - B) % Mod
  static constexpr UInt Sub(UInt A, UInt B, UInt Mod) noexcept {
    auto res = A - B;
    return A < B ? res + Mod : res;
  }
  // Return: (A * B) % Mod
  static constexpr UInt Mul(UInt A, UInt B, UInt Mod) noexcept {
    return ImplMA::Mul_<UInt, MulRes>::Mul(A, B, Mod);
  }
  // Return: Pow(A, B) % Mod
  static constexpr UInt Pow(UInt A, UInt B, UInt Mod) noexcept {
    UInt res = 1;
    while (B) {
      if (B & 1) res = Mul(res, A, Mod);
      A = Mul(A, A, Mod);
      B >>= 1;
    }
    return res;
  }
  // Require: Gcd(N, Mod) = 1
  // Return: Multiplicative inverse of an N with respect to Mod
  static constexpr UInt Inv(UInt N, UInt Mod) noexcept {
    return NrmS(GcdEx(N, Mod).x, Mod);
    // Pow(N, Mod - 2) => Mod must be prime
    // Pow(N, PrimeTable::Phi[Mod] - 1) => for arbitrary Mod
    // InversionTable::Inv[N] => Mod must be prime
  }
  // Require: Gcd(B, Mod) = 1
  // Return: (A / B) % Mod
  static constexpr UInt Div(UInt A, UInt B, UInt Mod) noexcept {
    return Mul(A, Inv(B, Mod), Mod);
  }
  // Generate multiplicative inverses for all 1 <= i <= N with respect to Mod
  // Require: Mod must be prime, N < Mod
  static constexpr void GenInvTable(UInt Inv[], UInt N, UInt Mod) noexcept {
    Inv[1] = 1;
    for (UInt i = 2; i <= N; ++i)
      Inv[i] = Mul(Mod - Mod / i, Inv[Mod % i], Mod);
  }
};
