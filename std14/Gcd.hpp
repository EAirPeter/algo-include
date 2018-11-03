// Greatest Common Divisor
#pragma once

#include <type_traits>

// Return: GCD of a and b
template<class Unsigned>
constexpr Unsigned Gcd(Unsigned A, Unsigned B) noexcept {
    while (B) {
        auto C = A % B;
        A = B;
        B = C;
    }
    return A;
}

// Impl: Result of GcdEx
template<class Unsigned_, class Signed_ = ::std::make_signed_t<Unsigned_>>
struct GcdRes {
    using Unsigned = Unsigned_;
    using Signed = Signed_;
    Unsigned d;
    Signed x;
    Signed y;
};

// Solve A * x + B * y = d; where d is Gcd(A, B)
// Return: {d, x, y}
template<class Unsigned, class Signed = ::std::make_signed_t<Unsigned>>
constexpr GcdRes<Unsigned, Signed> GcdEx(Unsigned A, Unsigned B) noexcept {
    if (!B)
        return {A, 1, 0};
    auto res = GcdEx<Unsigned, Signed>(B, A % B);
    return {res.d, res.y, res.x - res.y * (Signed) (A / B)};
}
