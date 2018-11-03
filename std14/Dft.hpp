// Discrete Fourier Transform
#pragma once

#include <cstddef>
#include <complex>
#include <utility>

// Discrete Fourier Transform
// Note: When considering only subscript in range [A.Len + B.Len - Len - 1, Len),
//   circular convolution is equivalent to linear convolution
//   where Len is the length of the transform
template<::std::size_t MaxExp_, class Complex_, class Real_ = typename Complex_::value_type>
struct Dft {
    static constexpr auto MaxExp = MaxExp_;
    static constexpr auto MaxLen = ::std::size_t{1} << MaxExp;
    using Complex = Complex_;
    using Real = Real_;
    // Only for static initialization
    // Use placement new before performing any transform
    constexpr Dft() noexcept = default;
    // Initialize resources needed for performing DFT and IDFT
    // Generate w table and its multiplication inversion table
    constexpr Dft(::std::size_t exp) noexcept : Exp(exp), Len(1u << Exp) {
        using ::std::acos;
        using ::std::conj;
        using ::std::cos;
        using ::std::sin;
        auto delta = acos((Real) -1) * 2 / (Real) Len;
        for (::std::size_t i = 0; i < Len; ++i) {
            WFwd[i] = Complex(cos(delta * (Real) i), sin(delta * (Real) i));
            WInv[i] = conj(WFwd[i]);
        }
    }
    // Perform DFT on sequence A
    // Require: The length of A must be Pow(2, Exp)
    constexpr void Forward(Complex A[]) const noexcept {
        XTransform(A, WFwd);
    }
    // Perform IDFT on sequence A
    // Require: The length of A must be Pow(2, Exp)
    constexpr void Inverse(Complex A[]) const noexcept {
        XTransform(A, WInv);
        for (::std::size_t i = 0; i < Len; ++i)
            A[i] /= (Real) Len;
    }
    constexpr void XTransform(Complex A[], const Complex W[]) const noexcept {
        using ::std::swap;
        for (::std::size_t i = 0, j = 0; i < Len; ++i) {
            if (i > j)
                swap(A[i], A[j]);
            for (::std::size_t k = Len >> 1; (j ^= k) < k; k >>= 1);
        }
        for (::std::size_t i = 0; i < Exp; ++i) {
            ::std::size_t ChkSiz = 1u << i;
            for (::std::size_t j = 0; j < Len; j += 2u << i)
                for (::std::size_t k = 0; k < ChkSiz; ++k) {
                    auto t = W[(Len >> (i + 1)) * k] * A[j + k + ChkSiz];
                    A[j + k + ChkSiz] = A[j + k] - t;
                    A[j + k] += t;
                }
        }
    }
    Complex WFwd[MaxLen]{};
    Complex WInv[MaxLen]{};
    ::std::size_t Exp = 0, Len = 0;
};
