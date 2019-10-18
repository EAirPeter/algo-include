#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

using namespace std;

// A proth prime and its primitive root
constexpr unsigned G = 3;
constexpr unsigned MDN = (479 << 21) + 1;

unsigned pwm(unsigned a, unsigned n) {
    unsigned x = 1;
    while (n) {
        if (n & 1)
            x = (unsigned) (((unsigned long long) x * a) % MDN);
        a = (unsigned) (((unsigned long long) a * a) % MDN);
        n >>= 1;
    }
    return x;
}

// NumberTheoreticTransform
// N shall be 2^k, V: false(NTT), true(INTT)
void ntt(unsigned *A, unsigned N, bool V) {
    for(unsigned i = 0; i < N; ++i) {
        unsigned r = 0;
        for(unsigned j = i, k = 1; k < N; j >>= 1, k <<= 1)
            r = (r << 1) | (j & 1);
        if(i < r)
            swap(A[i], A[r]);
    }
    for (unsigned h = 2; h <= N; h <<= 1) {
        unsigned wn = pwm(G, (MDN - 1) / h);
        for (unsigned i = 0; i < N; i += h) {
            unsigned w = 1;
            for (unsigned j = 0; j < (h >> 1); ++j) {
                unsigned t = (unsigned)
                    ((unsigned long long) w * A[i + j + (h >> 1)] % MDN);
                A[i + j + (h >> 1)] = (A[i + j] - t + MDN) % MDN;
                A[i + j] = (A[i + j] + t) % MDN;
                w = (unsigned) (((unsigned long long) w * wn) % MDN);
            }
        }
    }
    if (V) {
        for (unsigned i = 1; i < (N >> 1); ++i)
            swap(A[i], A[N - i]);
        unsigned inv = pwm(N, MDN - 2);
        for (unsigned i = 0; i < N; ++i)
            A[i] = (unsigned) (((unsigned long long) A[i] * inv) % MDN);
    }
}

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

int main() {
    using Int = unsigned;
    using Vec = ::std::vector<Int>;
    constexpr Int Mod = (Int(479) << 21) + 1;
    constexpr Int G = 3;
    Vec A, B, C;
    ReadVec(A);
    ReadVec(B);
    auto len = A.size() + B.size() - 1;
    auto exp = ::std::size_t{0};
    while (::std::size_t{1} << exp < len)
        ++exp;
    len = ::std::size_t{1} << ++exp;
    A.resize(len);
    B.resize(len);
    C.resize(len);
    PrintVec(A);
    PrintVec(B);
    ntt(A.data(), len, false);
    ntt(B.data(), len, false);
    PrintVec(A);
    PrintVec(B);
    for (::std::size_t i = 0; i < len; ++i)
        C[i] = (unsigned) ((unsigned long long) A[i] * B[i] % Mod);
    ntt(C.data(), len, true);
    PrintVec(C);
    return 0;
}
/*
unsigned gin(unsigned *A) {
    static char S[50001];
    if (scanf("%s", S) != 1)
        exit(0);
    unsigned L = (unsigned) strlen(S);
    for (unsigned i = 0; i < L; ++i)
        A[L - i - 1] = S[i] & 0x0f;
    return L;
}

unsigned A[131072], B[131072];

int main() {
    for (;;) {
        unsigned L1 = gin(A);
        unsigned L2 = gin(B);
        if (L1 < L2)
            L1 = L2;
        unsigned L;
        for (L = 1; L < (L1 << 1); L <<= 1);
        memset(A + L1, 0, sizeof(unsigned) * (L - L1));
        memset(B + L2, 0, sizeof(unsigned) * (L - L2));
        ntt(A, L, false);
        ntt(B, L, false);
        for (unsigned i = 0; i < L; ++i)
            A[i] = (unsigned) (((unsigned long long) A[i] * B[i]) % MDN);
        ntt(A, L, true);
        unsigned t = 0;
        for (unsigned i = 0; i < L; ++i) {
            A[i] += t;
            t = A[i] / 10;
            A[i] %= 10;
        }
        while (L && !A[L - 1])
            --L;
        if (!L)
            puts("0");
        else {
            while (L)
                printf("%01d", A[--L]);
            putchar('\n');
        }
    }
    return 0;
}
*/
