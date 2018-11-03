// 2D Vector
#pragma once

#include <iosfwd>

template<class Integral>
struct ScalarTraits {
    static constexpr Integral Sign(Integral val) noexcept {
        return val;
    }
};

template<>
struct ScalarTraits<float> {
    static constexpr int Sign(float val) noexcept {
        return val > 1e-6 ? 1 : val < -1e-6 ? -1 : 0;
    }
};

template<>
struct ScalarTraits<double> {
    static constexpr int Sign(double val) noexcept {
        return val > 1e-8 ? 1 : val < -1e-8 ? -1 : 0;
    }
};

template<class Scalar, class Traits = ScalarTraits<Scalar>>
struct Vec2 {
    Scalar x, y;
    Vec2() noexcept = default;
    constexpr Vec2(Scalar x_, Scalar y_) noexcept : x(x_), y(y_) {}
    constexpr bool operator <(const Vec2& rhs) const noexcept {
        return x < rhs.x ? true : x > rhs.x ? false : y < rhs.y;
    }
    constexpr bool operator ==(const Vec2& rhs) const noexcept {
        return x == rhs.x && y == rhs.y;
    }
    friend constexpr Vec2 operator *(Scalar lhs, const Vec2& rhs) noexcept {
        return {lhs * rhs.x, lhs * rhs.y};
    }
    constexpr Vec2 operator *(Scalar rhs) const noexcept {
        return {x * rhs, y * rhs};
    }
    constexpr Vec2 operator /(Scalar rhs) const noexcept {
        return {x / rhs, y / rhs};
    }
    constexpr Vec2 operator +(const Vec2& rhs) const noexcept {
        return {x + rhs.x, y + rhs.y};
    }
    constexpr Vec2 operator -(const Vec2& rhs) const noexcept {
        return {x - rhs.x, y - rhs.y};
    }
    constexpr Scalar operator *(const Vec2& rhs) const noexcept {
        return x * rhs.x + y * rhs.y;
    }
    constexpr Scalar operator %(const Vec2& rhs) const noexcept {
        return x * rhs.y - y * rhs.x;
    }
    constexpr Scalar Len2() const noexcept {
        return *this * *this;
    }
    constexpr bool IsInCircumCircle(const Vec2& A, const Vec2& B, const Vec2& C) noexcept {
        auto AP = *this - A;
        auto BP = *this - B;
        auto CP = *this - C;
        return Traits::Sign(AP % BP * CP.Len2() + BP % CP * AP.Len2() + CP % AP * BP.Len2()) > 0;
    }
    constexpr bool IsLeftOfVector(const Vec2& A, const Vec2& B) noexcept {
        auto AB = B - A;
        auto AP = *this - A;
        return Traits::Sign(AB % AP) > 0;
    }
    friend constexpr Vec2 CircumCenter(const Vec2& A, const Vec2& B, const Vec2& C) noexcept {
        auto AB = B - A;
        auto AC = C - A;
        auto lb = AB.Len2();
        auto lc = AC.Len2();
        auto den = (AB % AC) * 2;
        return A + Vec2{(AC.y * lb - AB.y * lc) / den, (AB.x * lc - AC.x * lb) / den};
    }
};

template<class Scalar>
inline ::std::istream& operator >>(::std::istream& is, Vec2<Scalar>& vec) noexcept {
    return is >> vec.x >> vec.y;
}
