// Number Theoretic Transform - AVX2
#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>

#include <cstdio>
#include <iostream>

#include <immintrin.h>

#ifdef _MSC_VER
#define restrict_ __restrict
#define inline_ inline __forceinline
#else // !defined(_MSC_VER)
#define restrict_ __restrict__
#define inline_ inline __attribute__((always_inline))
#endif // ifdef _MSC_VER

namespace ntt {

#ifdef NTT_USE_AVX2
inline
#endif
namespace avx2 {

using ::std::uint32_t;
using ::std::uint64_t;
using ::std::size_t;

template<int N>
using MkIntSeq = ::std::make_integer_sequence<int, N>;

template<int... Is>
using IntSeq = ::std::integer_sequence<int, Is...>;

template<uint32_t Mod, uint32_t G>
class Ntt {
public:
  template<int LogLen, class Exps = MkIntSeq<LogLen>>
  static void Forward(uint32_t* restrict_ A) noexcept {
    X_Fwd<LogLen, Pow(G, (Mod - 1) >> LogLen)>(A, Exps{});
  }

  template<int LogLen, class Exps = MkIntSeq<LogLen>>
  static void Inverse(uint32_t* restrict_ A) noexcept {
    X_Inv<LogLen, Pow(X_InvG, (Mod - 1) >> LogLen)>(A, Exps{});
  }

  static constexpr uint32_t Min(uint32_t A, uint32_t B) noexcept {
    return A < B ? A : B;
  }

  static constexpr uint32_t Add(uint32_t A, uint32_t B) noexcept {
    return Min(A + B, A + B - Mod);
  }

  static constexpr uint32_t Sub(uint32_t A, uint32_t B) noexcept {
    return Min(A - B, A - B + Mod);
  }

  static constexpr uint32_t Mul(uint32_t A, uint32_t B) noexcept {
    return (uint32_t) ((uint64_t) A * B % Mod);
  }

  static constexpr uint32_t Pow(uint32_t A, size_t B) noexcept {
    uint32_t Res = 1;
    while (B) {
      if (B & 1)
        Res = Mul(Res, A);
      A = Mul(A, A);
      B >>= 1;
    }
    return Res;
  }

  static constexpr uint32_t Inv(uint32_t A) noexcept {
    return Pow(A, Mod - 2);
  }

private:
  static constexpr uint32_t X_InvG = Inv(G);

  template<int LogLen, uint32_t Wn, int... Exps>
  static inline_ void X_Fwd(uint32_t* restrict_ A, IntSeq<Exps...>) noexcept {
    if constexpr (LogLen > 0) {
      (..., X_FwdAt<
        LogLen,
        LogLen - Exps - 1,
        Pow(Wn, size_t{1} << Exps)
      >(A));
    }
  }

  template<int LogLen, uint32_t Wn, int... Exps>
  static inline_ void X_Inv(uint32_t* restrict_ A, IntSeq<Exps...>) noexcept {
    if constexpr (LogLen > 0) {
      (..., X_InvAt<
        LogLen,
        Exps,
        Pow(Wn, size_t{1} << (LogLen - Exps - 1))
      >(A));
      constexpr uint32_t InvLen = Inv(
        (uint32_t) ((size_t{1} << LogLen) % Mod)
      );
      VMul<LogLen>(A, InvLen);
    }
  }

public:
  template<int LogLen>
  static inline_ void VAdd(uint32_t* restrict_ A,
    const uint32_t* restrict_ B) noexcept
  {
    if constexpr (LogLen <= 0) {
      A[0] = Add(A[0], B[0]);
    }
    else if constexpr (LogLen == 1) {
      A[0] = Add(A[0], B[0]);
      A[1] = Add(A[1], B[1]);
    }
    else if constexpr (LogLen == 2) {
      X_VStoX(A, X_VAddX(X_VLodX(A), X_VLodX(B)));
    }
    else if constexpr (LogLen < X_Ubll) {
      constexpr auto Len = size_t{1} << LogLen;
      for (size_t i = 0; i < Len; i += 8)
        X_VStoY(A + i, X_VAddY(X_VLodY(A + i), X_VLodY(B + i)));
    }
  }

  template<int LogLen>
  static inline_ void VAdd(uint32_t* restrict_ A, uint32_t B) noexcept {
    if constexpr (LogLen <= 0) {
      A[0] = Add(A[0], B);
    }
    else if constexpr (LogLen == 1) {
      A[0] = Add(A[0], B);
      A[1] = Add(A[1], B);
    }
    else if constexpr (LogLen == 2) {
      X_VStoX(A, X_VAddX(X_VLodX(A), _mm_set1_epi32(B)));
    }
    else if constexpr (LogLen < X_Ubll) {
      constexpr auto Len = size_t{1} << LogLen;
      auto vb = _mm256_set1_epi32(B);
      for (size_t i = 0; i < Len; i += 8)
        X_VStoY(A + i, X_VAddY(X_VLodY(A + i), vb));
    }
  }

  template<int LogLen>
  static inline_ void VAdd(uint32_t* restrict_ A) noexcept
  {
    if constexpr (LogLen <= 0) {
      A[0] = Add(A[0], A[0]);
    }
    else if constexpr (LogLen == 1) {
      A[0] = Add(A[0], A[0]);
      A[1] = Add(A[1], A[1]);
    }
    else if constexpr (LogLen == 2) {
      auto va = X_VLodX(A);
      X_VStoX(A, X_VAddX(va, va));
    }
    else if constexpr (LogLen < X_Ubll) {
      constexpr auto Len = size_t{1} << LogLen;
      for (size_t i = 0; i < Len; i += 8) {
        auto va = X_VLodY(A + i);
        X_VStoY(A + i, X_VAddY(va, va));
      }
    }
  }

  template<int LogLen>
  static inline_ void VSub(uint32_t* restrict_ A,
    const uint32_t* restrict_ B) noexcept
  {
    if constexpr (LogLen <= 0) {
      A[0] = Sub(A[0], B[0]);
    }
    else if constexpr (LogLen == 1) {
      A[0] = Sub(A[0], B[0]);
      A[1] = Sub(A[1], B[1]);
    }
    else if constexpr (LogLen == 2) {
      X_VStoX(A, X_VSubX(X_VLodX(A), X_VLodX(B)));
    }
    else if constexpr (LogLen < X_Ubll) {
      constexpr auto Len = size_t{1} << LogLen;
      for (size_t i = 0; i < Len; i += 8)
        X_VStoY(A + i, X_VSubY(X_VLodY(A + i), X_VLodY(B + i)));
    }
  }

  template<int LogLen>
  static inline_ void VSub(uint32_t* restrict_ A, uint32_t B) noexcept {
    if constexpr (LogLen <= 0) {
      A[0] = Sub(A[0], B);
    }
    else if constexpr (LogLen == 1) {
      A[0] = Sub(A[0], B);
      A[1] = Sub(A[1], B);
    }
    else if constexpr (LogLen == 2) {
      X_VStoX(A, X_VSubX(X_VLodX(A), _mm_set1_epi32(B)));
    }
    else if constexpr (LogLen < X_Ubll) {
      constexpr auto Len = size_t{1} << LogLen;
      auto vb = _mm256_set1_epi32(B);
      for (size_t i = 0; i < Len; i += 8)
        X_VStoY(A + i, X_VSubY(X_VLodY(A + i), vb));
    }
  }

  template<int LogLen>
  static inline_ void VMul(uint32_t* restrict_ A,
    const uint32_t* restrict_ B) noexcept
  {
    if constexpr (LogLen <= 0) {
      A[0] = Mul(A[0], B[0]);
    }
    else if constexpr (LogLen == 1) {
      A[0] = Mul(A[0], B[0]);
      A[1] = Mul(A[1], B[1]);
    }
    else if constexpr (LogLen == 2) {
      X_VStoX(A, X_VMulX(X_VLodX(A), X_VLodX(B)));
    }
    else if constexpr (LogLen < X_Ubll) {
      constexpr auto Len = size_t{1} << LogLen;
      for (size_t i = 0; i < Len; i += 8)
        X_VStoY(A + i, X_VMulY(X_VLodY(A + i), X_VLodY(B + i)));
    }
  }

  template<int LogLen>
  static inline_ void VMul(uint32_t* restrict_ A, uint32_t B) noexcept {
    if constexpr (LogLen <= 0) {
      A[0] = Mul(A[0], B);
    }
    else if constexpr (LogLen == 1) {
      A[0] = Mul(A[0], B);
      A[1] = Mul(A[1], B);
    }
    else if constexpr (LogLen == 2) {
      X_VStoX(A, X_VMulX(X_VLodX(A), _mm256_set1_epi64x(B)));
    }
    else if constexpr (LogLen < X_Ubll) {
      constexpr auto Len = size_t{1} << LogLen;
      auto vb = _mm256_set1_epi64x(B);
      for (size_t i = 0; i < Len; i += 8)
        X_VStoY(A + i, X_VMulY(X_VLodY(A + i), vb, vb));
    }
  }

  template<int LogLen>
  static inline_ void VMul(uint32_t* restrict_ A) noexcept {
    if constexpr (LogLen <= 0) {
      A[0] = Mul(A[0], A[0]);
    }
    else if constexpr (LogLen == 1) {
      A[0] = Mul(A[0], A[0]);
      A[1] = Mul(A[1], A[1]);
    }
    else if constexpr (LogLen == 2) {
      auto va = _mm256_cvtepu32_epi64(X_VLodX(A));
      X_VStoX(A, X_VMulX(va, va));
    }
    else if constexpr (LogLen < X_Ubll) {
      constexpr auto Len = size_t{1} << LogLen;
      for (size_t i = 0; i < Len; i += 8) {
        auto va0 = X_VLodY(A + i);
        auto va1 = _mm256_shuffle_epi32(va0, 0xf5); // 0q3311
        X_VStoY(A + i, X_VMulY(va0, va1, va0, va1));
      }
    }
  }

private:
  template<int LogLen, int Exp, uint32_t Wn>
  static constexpr void X_FwdAt(uint32_t* restrict_ A) noexcept {
    constexpr auto Len = size_t{1} << LogLen;
    if constexpr (Exp <= 0) {
      if constexpr (LogLen <= 0) {}
      else if constexpr (LogLen == 1) {
        auto a = Add(A[0], A[1]);
        auto b = Sub(A[0], A[1]);
        A[0] = a;
        A[1] = b;
      }
      else if constexpr (LogLen == 2) {
        // 0x2x + 1x3x => 0123 + 1032
        // x0x2 - x1x3 => 1032 - 0123
        auto va = X_VLodX(A);
        auto vb = _mm_shuffle_epi32(va, 0xb1); // 0q2301
        auto vc0 = _mm_add_epi32(va, vb);
        auto vc1 = _mm_sub_epi32(vb, va);
        auto vc = _mm_blend_epi32(vc0, vc1, 0xa); // 0b1010
        auto vcm = _mm_add_epi32(vc, x_vnmnmx32);
        auto vx = _mm_min_epu32(vc, vcm);
        X_VStoX(A, vx);
      }
      else if constexpr (LogLen < X_Ubll) {
        for (size_t i = 0; i < Len; i += 8) {
          auto va = X_VLodY(A + i);
          auto vb = _mm256_shuffle_epi32(va, 0xb1); // 0q2301
          auto vc0 = _mm256_add_epi32(va, vb);
          auto vc1 = _mm256_sub_epi32(vb, va);
          auto vc = _mm256_blend_epi32(vc0, vc1, 0xaa); // 0b10101010
          auto vcm = _mm256_add_epi32(vc, x_vnmnmy32);
          auto vx = _mm256_min_epu32(vc, vcm);
          X_VStoY(A + i, vx);
        }
      }
    }
    else if constexpr (Exp == 1) {
      if constexpr (LogLen <= 1) {}
      else if constexpr (LogLen == 2) {
        // 01xx + 23xx => 0123 + 2301
        // xx01 - xx23 => 2301 - 0123
        auto va = X_VLodX(A);
        auto vb = _mm_shuffle_epi32(va, 0x4e); // 0q1032
        auto vc0 = _mm_add_epi32(va, vb);
        auto vc1 = _mm_sub_epi32(vb, va);
        auto vc = _mm_blend_epi32(vc0, vc1, 0xc); // 0b1100
        auto vcm = _mm_add_epi32(vc, x_vnnmmx32);
        auto vx = _mm_min_epu32(vc, vcm);
        X_VStoX(A, vx);
        A[3] = Mul(A[3], Wn);
      }
      else if constexpr (LogLen == 3) {
        auto va = X_VLodY(A);
        auto vb = _mm256_shuffle_epi32(va, 0x4e); // 0q1032
        auto vc0 = _mm256_add_epi32(va, vb);
        auto vc1 = _mm256_sub_epi32(vb, va);
        auto vc = _mm256_blend_epi32(vc0, vc1, 0xcc); // 0b11001100
        auto vcm = _mm256_add_epi32(vc, x_vnnmmy32);
        auto vx = _mm256_min_epu32(vc, vcm);
        X_VStoY(A, vx);
        A[3] = Mul(A[3], Wn);
        A[7] = Mul(A[7], Wn);
      }
      else if constexpr (LogLen < X_Ubll) {
        const auto vwn = _mm256_set1_epi64x(Wn);
        for (size_t i = 0; i < Len; i += 16) {
          auto va0 = X_VLodY(A + i + 0);
          auto va1 = X_VLodY(A + i + 8);
          auto vb0 = _mm256_shuffle_epi32(va0, 0x4e); // 0q1032
          auto vb1 = _mm256_shuffle_epi32(va1, 0x4e); // 0q1032
          auto vc00 = _mm256_add_epi32(va0, vb0);
          auto vc01 = _mm256_sub_epi32(vb0, va0);
          auto vc10 = _mm256_add_epi32(va1, vb1);
          auto vc11 = _mm256_sub_epi32(vb1, va1);
          auto vc0 = _mm256_blend_epi32(vc00, vc01, 0xcc); // 0b11001100
          auto vc1 = _mm256_blend_epi32(vc10, vc11, 0xcc); // 0b11001100
          auto vcm0 = _mm256_add_epi32(vc0, x_vnnmmy32);
          auto vcm1 = _mm256_add_epi32(vc1, x_vnnmmy32);
          auto vx0 = _mm256_min_epu32(vc0, vcm0);
          auto vx1 = _mm256_min_epu32(vc1, vcm1);
          auto vy0 = _mm256_shuffle_epi32(vx0, 0xff); // 0q3333
          auto vy1 = _mm256_shuffle_epi32(vx1, 0xff); // 0q3333
          auto vy = _mm256_blend_epi32(vy0, vy1, 0xcc); // 0b11001100
          auto vz = X_VMulYp(vy, vwn);
          auto vz0 = _mm256_shuffle_epi32(vz, 0x00); // 0q0000
          auto vz1 = _mm256_shuffle_epi32(vz, 0xaa); // 0q2222
          auto vw0 = _mm256_blend_epi32(vx0, vz0, 0x88); // 0b10001000
          auto vw1 = _mm256_blend_epi32(vx1, vz1, 0x88); // 0b10001000
          X_VStoY(A + i + 0, vw0);
          X_VStoY(A + i + 8, vw1);
        }
      }
    }
    else if constexpr (Exp == 2) {
      constexpr auto Wn2 = Mul(Wn, Wn);
      constexpr auto Wn3 = Mul(Wn2, Wn);
      if constexpr (LogLen <= 2) {}
      else if constexpr (LogLen == 3) {
        const auto vw = _mm256_set_epi64x(Wn3, Wn2, Wn, 1);
        auto va = X_VLodX(A + 0);
        auto vb = X_VLodX(A + 4);
        auto vc = X_VSubX(va, vb);
        auto vd = X_VAddX(va, vb);
        auto ve = X_VMulX(vc, vw);
        X_VStoX(A + 0, vd);
        X_VStoX(A + 4, ve);
      }
      else if constexpr (LogLen < X_Ubll) {
        const auto vw0 = _mm256_set_epi64x(Wn2, 1, Wn2, 1);
        const auto vw1 = _mm256_set_epi64x(Wn3, Wn, Wn3, Wn);
        for (size_t i = 0; i < Len; i += 16) {
          auto va0 = X_VLodY(A + i + 0);
          auto va_ = _mm256_loadu_si256(
            (const __m256i* restrict_) (A + i + 4));
          auto va1 = X_VLodY(A + i + 8);
          auto va = _mm256_blend_epi32(va0, va_, 0xf0); // 0b11110000
          auto vb = _mm256_blend_epi32(va_, va1, 0xf0); // 0b11110000
          auto vc = X_VSubY(va, vb);
          auto vd0 = X_VAddY(va, vb);
          auto ve1 = X_VMulY(vc, vw0, vw1);
          auto vd1 = _mm256_permute4x64_epi64(vd0, 0xee); // 0q3232
          auto ve0 = _mm256_permute4x64_epi64(ve1, 0x44); // 0q1010
          auto vx1 = _mm256_blend_epi32(vd1, ve1, 0xf0); // 0b11110000
          auto vx0 = _mm256_blend_epi32(vd0, ve0, 0xf0); // 0b11110000
          X_VStoY(A + i + 8, vx1);
          X_VStoY(A + i + 0, vx0);
        }
      }
    }
    else if constexpr (Exp < LogLen) {
      constexpr auto Wn2 = Mul(Wn, Wn);
      constexpr auto Wn3 = Mul(Wn2, Wn);
      constexpr auto Wn4 = Mul(Wn3, Wn);
      constexpr auto Wn5 = Mul(Wn4, Wn);
      constexpr auto Wn6 = Mul(Wn5, Wn);
      constexpr auto Wn7 = Mul(Wn6, Wn);
      constexpr auto Wnn = Mul(Wn7, Wn);
      if constexpr (LogLen <= 3) {}
      else if constexpr (LogLen == 4) {
        const auto vw0 = _mm256_set_epi64x(Wn6, Wn4, Wn2, 1);
        const auto vw1 = _mm256_set_epi64x(Wn7, Wn5, Wn3, Wn);
        auto va = X_VLodY(A + 0);
        auto vb = X_VLodY(A + 8);
        auto vc = X_VSubY(va, vb);
        auto vd = X_VAddY(va, vb);
        auto ve = X_VMulY(vc, vw0, vw1);
        X_VStoY(A + 0, vd);
        X_VStoY(A + 8, ve);
      }
      else if constexpr (LogLen < X_Ubll) {
        constexpr auto Leap = size_t{2} << Exp;
        constexpr auto Step = size_t{1} << Exp;
        const auto vwn = _mm256_set1_epi64x(Wnn);
        for (size_t i = 0; i < Len; i += Leap) {
          auto vw0 = _mm256_set_epi64x(Wn6, Wn4, Wn2, 1);
          auto vw1 = _mm256_set_epi64x(Wn7, Wn5, Wn3, Wn);
          for (size_t j = i; j < i + Step; j += 8) {
            auto va = X_VLodY(A + j);
            auto vb = X_VLodY(A + j + Step);
            auto vc = X_VSubY(va, vb);
            auto vd = X_VAddY(va, vb);
            auto ve = X_VMulY(vc, vw0, vw1);
            X_VStoY(A + j, vd);
            X_VStoY(A + j + Step, ve);
            vw0 = X_VMulY(vw0, vw1, vwn, vwn);
            vw1 = _mm256_shuffle_epi32(vw0, 0xf5); // 0q3311
          }
        }
      }
    }
  }

  template<int LogLen, int Exp, uint32_t Wn>
  static constexpr void X_InvAt(uint32_t* restrict_ A) noexcept {
    constexpr auto Len = size_t{1} << LogLen;
    constexpr auto Leap = size_t{2} << Exp;
    constexpr auto Step = size_t{1} << Exp;
    for (size_t i = 0; i < Len; i += Leap) {
      uint32_t W = 1;
      for (size_t j = i; j < i + Step; ++j) {
        auto A_ = A[j];
        auto B_ = Mul(W, A[j + Step]);
        A[j] = Add(A_, B_);
        A[j + Step] = Sub(A_, B_);
        W = Mul(W, Wn);
      }
    }
  }

  static inline_ __m128i X_VLodX(const uint32_t* restrict_ A) noexcept {
    return _mm_load_si128((const __m128i* restrict_) A);
  }

  static inline_ void X_VStoX(uint32_t* restrict_ A, __m128i v) noexcept {
    _mm_store_si128((__m128i* restrict_) A, v);
  }

  static inline_ __m256i X_VLodY(const uint32_t* restrict_ A) noexcept {
    return _mm256_load_si256((const __m256i* restrict_) A);
  }

  static inline_ void X_VStoY(uint32_t* restrict_ A, __m256i v) noexcept {
    _mm256_store_si256((__m256i* restrict_) A, v);
  }

  static inline_ __m128i X_VAddX(__m128i va, __m128i vb) noexcept {
    auto vta = _mm_add_epi32(va, vb);
    auto vtb = _mm_sub_epi32(vta, x_vmx32);
    return _mm_min_epu32(vta, vtb);
  }

  static inline_ __m256i X_VAddY(__m256i va, __m256i vb) noexcept {
    auto vta = _mm256_add_epi32(va, vb);
    auto vtb = _mm256_sub_epi32(vta, x_vmy32);
    return _mm256_min_epu32(vta, vtb);
  }

  static inline_ __m128i X_VSubX(__m128i va, __m128i vb) noexcept {
    auto vta = _mm_sub_epi32(va, vb);
    auto vtb = _mm_add_epi32(vta, x_vmx32);
    return _mm_min_epu32(vta, vtb);
  }

  static inline_ __m256i X_VSubY(__m256i va, __m256i vb) noexcept {
    auto vta = _mm256_sub_epi32(va, vb);
    auto vtb = _mm256_add_epi32(vta, x_vmy32);
    return _mm256_min_epu32(vta, vtb);
  }

  // [0123] = [0123] * [0123]
  static inline_ __m128i X_VMulX(__m128i va, __m128i vb) noexcept {
    return X_VMulX(va, _mm256_cvtepu32_epi64(vb));
  }

  // [0123] = [0123] * [0_1_2_3_]
  static inline_ __m128i X_VMulX(__m128i va, __m256i vb) noexcept {
    return X_VMulX(_mm256_cvtepu32_epi64(va), vb);
  }

  // [0123] = [0_1_2_3_] * [0_1_2_3_]
  static inline_ __m128i X_VMulX(__m256i va, __m256i vb) noexcept {
    auto vc = _mm256_mul_epu32(va, vb);
    auto vd = _mm256_srli_epi64(vc, X_F);
    auto vr = _mm256_srli_epi64(_mm256_mul_epu32(vd, x_vhy64), X_G);
    auto vx_ = _mm256_sub_epi32(vc, _mm256_mul_epu32(vr, x_vmy64));
    auto vx = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(vx_, x_vpy));
    auto vxm = _mm_sub_epi32(vx, x_vmx32);
    auto vxm2 = _mm_sub_epi32(vx, x_vm2x32);
    return _mm_min_epu32(_mm_min_epu32(vx, vxm), vxm2);
  }

  // [01234567] = [01234567] * [01234567]
  static inline_ __m256i X_VMulY(__m256i va, __m256i vb) noexcept {
    return X_VMulY(va, vb, _mm256_shuffle_epi32(vb, 0xf5)); // 0q3311
  }

  // [01234567] = [01234567] * ([0_2_4_6_], [1_3_5_7_])
  static inline_ __m256i X_VMulY(
    __m256i va, __m256i vb0, __m256i vb1) noexcept
  {
    return X_VMulY(va, _mm256_shuffle_epi32(va, 0xf5), vb0, vb1); // 0q3311
  }

  // [01234567] = ([0_2_4_6_], [1_3_5_7_]) * ([0_2_4_6_], [1_3_5_7_])
  static inline_ __m256i X_VMulY(
    __m256i va0, __m256i va1, __m256i vb0, __m256i vb1) noexcept
  {
    auto vc0 = _mm256_mul_epu32(va0, vb0);
    auto vc1 = _mm256_mul_epu32(va1, vb1);
    auto vd0 = _mm256_srli_epi64(vc0, X_F);
    auto vd1 = _mm256_srli_epi64(vc1, X_F);
    auto vr0 = _mm256_srli_epi64(_mm256_mul_epu32(vd0, x_vhy64), X_G);
    auto vr1 = _mm256_srli_epi64(_mm256_mul_epu32(vd1, x_vhy64), X_G);
    auto vs0 = _mm256_mul_epu32(vr0, x_vmy64);
    auto vs1 = _mm256_mul_epu32(vr1, x_vmy64);
    //                                                       0q2200 0b10101010
    auto vc = _mm256_blend_epi32(vc0, _mm256_shuffle_epi32(vc1, 0xa0), 0xaa);
    auto vs = _mm256_blend_epi32(vs0, _mm256_shuffle_epi32(vs1, 0xa0), 0xaa);
    auto vx = _mm256_sub_epi32(vc, vs);
    auto vxm = _mm256_sub_epi32(vx, x_vmy32);
    auto vxm2 = _mm256_sub_epi32(vx, x_vm2y32);
    return _mm256_min_epu32(_mm256_min_epu32(vx, vxm), vxm2);
  }

  // [0_1_2_3_] = [0_1_2_3_] * [0_1_2_3_]
  static inline_ __m256i X_VMulYp(__m256i va, __m256i vb) noexcept {
    auto vc = _mm256_mul_epu32(va, vb);
    auto vd = _mm256_srli_epi64(vc, X_F);
    auto vr = _mm256_srli_epi64(_mm256_mul_epu32(vd, x_vhy64), X_G);
    auto vs = _mm256_mul_epu32(vr, x_vmy64);
    auto vx = _mm256_sub_epi32(vc, vs);
    auto vxm = _mm256_sub_epi32(vx, x_vmy32);
    auto vxm2 = _mm256_sub_epi32(vx, x_vm2y32);
    return _mm256_min_epu32(_mm256_min_epu32(vx, vxm), vxm2);
  }

  static inline_ void X_VDump(__m128i v, const char* S = "") noexcept {
    alignas(32) union {
      uint32_t D[4];
      uint64_t Q[2];
    } U;
    _mm_store_si128((__m128i*) &U, v);
    printf(
      "%s: [%u,%u,%u,%u] [%llu,%llu]\n",
      S,
      U.D[0], U.D[1], U.D[2], U.D[3],
      U.Q[0], U.Q[1]
    );
  }

  static inline_ void X_VDump(__m256i v, const char* S = "") noexcept {
    alignas(32) union {
      uint32_t D[8];
      uint64_t Q[4];
    } U;
    _mm256_store_si256((__m256i*) &U, v);
    printf(
      "%s: [%u,%u,%u,%u,%u,%u,%u,%u] [%llu,%llu,%llu,%llu]\n",
      S,
      U.D[0], U.D[1], U.D[2], U.D[3], U.D[4], U.D[5], U.D[6], U.D[7],
      U.Q[0], U.Q[1], U.Q[2], U.Q[3]
    );
  }

  static constexpr int X_MGetF() noexcept {
    int F = 0;
    while (F < 32 && (1u << (F + 1)) <= Mod)
      ++F;
    return F;
  }

  static constexpr int X_MGetG(int F) noexcept {
    auto M2 = (uint64_t) (Mod - 1) * (Mod - 1);
    int G = 0;
    while (F + G < 64 && (1ull << (F + G)) < M2)
      ++G;
    return G;
  }

  static constexpr uint32_t X_MGetH(int F, int G) noexcept {
    return (uint32_t) ((uint64_t{1} << (F + G)) / Mod);
  }

  static constexpr int X_Ubll = ::std::numeric_limits<size_t>::digits;
  static constexpr int X_F = X_MGetF();
  static constexpr int X_G = X_MGetG(X_F);
  static constexpr int X_H = X_MGetH(X_F, X_G);

  static inline const auto x_vmx32 = _mm_set1_epi32(Mod);
  static inline const auto x_vm2x32 = _mm_set1_epi32(Mod + Mod);
  static inline const auto x_vnmnmx32 = _mm_set_epi32(Mod, -Mod, Mod, -Mod);
  static inline const auto x_vnnmmx32 = _mm_set_epi32(Mod, Mod, -Mod, -Mod);

  static inline const auto x_vmy32 = _mm256_set1_epi32(Mod);
  static inline const auto x_vm2y32 = _mm256_set1_epi32(Mod + Mod);
  static inline const auto x_vnmnmy32 = _mm256_set_epi32(
    Mod, -Mod, Mod, -Mod, Mod, -Mod, Mod, -Mod);
  static inline const auto x_vnnmmy32 = _mm256_set_epi32(
    Mod, Mod, -Mod, -Mod, Mod, Mod, -Mod, -Mod);
  static inline const auto x_vmy64 = _mm256_set1_epi64x(Mod);
  static inline const auto x_vhy64 = _mm256_set1_epi64x(X_H);
  static inline const auto x_vpy = _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0);

};

}} // namespace ntt::avx2
