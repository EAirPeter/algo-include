#include <complex>
#include <utility>

template<class tComplex, ::std::size_t kMaxL>
struct FastFourierTransform {
    using Complex = tComplex;
    using Real = typename Complex::value_type;

    constexpr static ::std::size_t MaxL = kMaxL;
    constexpr static ::std::size_t MaxN = ::std::size_t{1} << MaxL;

    const ::std::size_t exp, len;
    Complex fwdW[MaxN];
    Complex invW[MaxN];

    FastFourierTransform(::std::size_t exp_) noexcept :
        exp(exp_), len(::std::size_t{1} << exp_)
    {
        using ::std::acos;
        using ::std::conj;
        using ::std::cos;
        using ::std::sin;
        auto delta = acos(static_cast<Real>(-1.0)) * static_cast<Real>(2.0) / static_cast<Real>(len);
        for (::std::size_t i = 0; i < len; ++i) {
            fwdW[i] = Complex(cos(delta * static_cast<Real>(i)), sin(delta * static_cast<Real>(i)));
            invW[i] = conj(fwdW[i]);
        }
    }

    void Forward(Complex* A) const noexcept {
        XTransform(A, fwdW);
    }

    void Inverse(Complex* A) const noexcept {
        XTransform(A, invW);
        for (::std::size_t i = 0; i < len; ++i)
            A[i] /= static_cast<Real>(len);
    }

    void XTransform(Complex* A, const Complex* W) const noexcept {
        using ::std::swap;
        for (::std::size_t i = 0, j = 0; i < len; ++i) {
            if (i > j)
                swap(A[i], A[j]);
            for (::std::size_t k = len >> 1; (j ^= k) < k; k >>= 1);
        }
        for (::std::size_t i = 0; i < exp; ++i) {
            auto zChunk = ::std::size_t{1} << i;
            for (::std::size_t j = 0; j < len; j += ::std::size_t{2} << i) {
                for (::std::size_t k = 0; k < zChunk; ++k) {
                    auto t = W[(len >> (i + 1)) * k] * A[j + k + zChunk];
                    A[j + k + zChunk] = A[j + k] - t;
                    A[j + k] += t;
                }
            }
        }
    }
};

template<class tIntegral, ::std::size_t kMaxL, tIntegral kMod, tIntegral kG>
struct NumberTheoreticTransform {
    using Integral = tIntegral;

    constexpr static ::std::size_t MaxL = kMaxL;
    constexpr static ::std::size_t MaxN = ::std::size_t{1} << MaxL;
    constexpr static Integral Mod = kMod;
    constexpr static Integral G = kG;

    const ::std::size_t exp, len;
    const Integral invlen;
    Integral fwdW[MaxN];
    Integral invW[MaxN];

    NumberTheoreticTransform(::std::size_t exp_) noexcept :
        exp(exp_), len(::std::size_t{1} << exp_), invlen(XPowMod(static_cast<Integral>(len), Mod - 2))
    {
        auto fwd = XPowMod(G, (Mod - 1) >> exp);
        fwdW[0] = 1;
        for (::std::size_t i = 1; i < len; ++i)
            fwdW[i] = fwdW[i - 1] * fwd % Mod;
        auto inv = XPowMod(fwd, Mod - 2);
        invW[0] = 1;
        for (::std::size_t i = 1; i < len; ++i)
            invW[i] = invW[i - 1] * inv % Mod;
    }

    template<class tStor>
    void Forward(tStor* A) const noexcept {
        XTransform<tStor>(A, fwdW);
    }

    template<class tStor>
    void Inverse(tStor* A) const noexcept {
        XTransform<tStor>(A, invW);
        for (::std::size_t i = 0; i < len; ++i)
            A[i] = static_cast<tStor>((static_cast<Integral>(A[i]) * invlen) % Mod);
    }

    template<class tStor>
    void XTransform(tStor* A, const Integral* W) const noexcept {
        using ::std::swap;
        for (::std::size_t i = 0, j = 0; i < len; ++i) {
            if (i > j)
                swap(A[i], A[j]);
            for (::std::size_t k = len >> 1; (j ^= k) < k; k >>= 1);
        }
        for (::std::size_t i = 0; i < exp; ++i) {
            auto zChunk = ::std::size_t{1} << i;
            for (::std::size_t j = 0; j < len; j += ::std::size_t{2} << i) {
                for (::std::size_t k = 0; k < zChunk; ++k) {
                    auto t = (W[(len >> (i + 1)) * k] * static_cast<Integral>(A[j + k + zChunk])) % Mod;
                    A[j + k + zChunk] = static_cast<tStor>((static_cast<Integral>(A[j + k]) + Mod - t) % Mod);
                    A[j + k] = static_cast<tStor>((static_cast<Integral>(A[j + k]) + t) % Mod);
                }
            }
        }
    }

    static Integral XPowMod(Integral base, Integral exp) {
        Integral res{1};
        while (exp) {
            if (exp & 1)
                res = (res * base) % Mod;
            base = (base * base) % Mod;
            exp >>= 1;
        }
        return res;
    }
};

// Proth number; OEIS A080075; Mod = K * Pow(2, N) + 1
// Mod                  K   N   G
// 3                    1   1   2
// 5                    1   2   2
// 17                   1   4   3
// 97                   3   5   5
// 193                  3   6   5
// 257                  1   8   3
// 7681                 15  9   17
// 12289                3   12  11
// 40961                5   13  3
// 65537                1   16  3
// 786433               3   18  10
// 5767169              11  19  3
// 7340033              7   20  3
// 23068673             11  21  3
// 104857601            25  22  3
// 167772161            5   25  3
// 469762049            7   26  3
// 998244353            119 23  3
// 1004535809           479 21  3
// 2013265921           15  27  31
// 2281701377           17  27  3
// 3221225473           3   30  5
// 75161927681          35  31  3
// 77309411329          9   33  7
// 206158430209         3   36  22
// 2061584302081        15  37  7
// 2748779069441        5   39  3
// 6597069766657        3   41  5
// 39582418599937       9   42  5
// 79164837199873       9   43  5
// 263882790666241      15  44  7
// 1231453023109121     35  45  3
// 1337006139375617     19  46  3
// 3799912185593857     27  47  5
// 4222124650659841     15  48  19
// 7881299347898369     7   50  6
// 31525197391593473    7   52  3
// 180143985094819841   5   55  6
// 1945555039024054273  27  56  5
// 4179340454199820289  29  57  3

#include <cstdint>
#include <iostream>
#include <vector>

template<class Vec>
void ReadVec(Vec& vec) {
    using Elem = typename Vec::value_type;
    ::std::size_t size;
    ::std::cin >> size;
    while (size--) {
        Elem val;
        ::std::cin >> val;
        vec.emplace_back(::std::move(val));
    }
}

template<class Vec>
void PrintVec(const Vec& vec) {
    for (auto& val : vec)
        ::std::cout << val << ' ';
    ::std::cout << ::std::endl;
}

struct Fft {
    using Intm = ::std::complex<double>;
    using Stor = ::std::complex<double>;
    using Tran = FastFourierTransform<Intm, 12>;
    static Stor Mul(const Stor& lhs, const Stor& rhs) noexcept {
        return lhs * rhs;
    }
};

struct Ntt {
    using Intm = ::std::uint64_t;
    using Stor = ::std::uint64_t;
    constexpr static Intm Mod = (static_cast<Intm>(479) << 21) + 1;
    constexpr static Intm G = 3;
    using Tran = NumberTheoreticTransform<Intm, 12, Mod, G>;
    static Stor Mul(const Stor& lhs, const Stor& rhs) noexcept {
        return static_cast<Stor>(static_cast<Intm>(lhs) * static_cast<Intm>(rhs) % Mod);
    }
};

int main() {
    using T = Ntt;
    using Vec = ::std::vector<T::Stor>;
    Vec A, B, C;
    ReadVec(A);
    ReadVec(B);
    auto len = A.size() + B.size() - 1;
    auto exp = ::std::size_t{0};
    while ((::std::size_t{1} << exp) < len)
        ++exp;
    len = ::std::size_t{1} << ++exp;
    T::Tran t(exp);
    A.resize(len);
    B.resize(len);
    C.resize(len);
    PrintVec(A);
    PrintVec(B);
    t.Forward(A.data());
    t.Forward(B.data());
    PrintVec(A);
    PrintVec(B);
    for (::std::size_t i = 0; i < len; ++i)
        C[i] = T::Mul(A[i], B[i]);
    t.Inverse(C.data());
    PrintVec(C);
    return 0;
}
