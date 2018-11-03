#include <algorithm>
#include <utility>

using namespace std;

typedef unsigned long long U64;
typedef unsigned U32;

#define NoExcept throw()

const U32 Mod = (3ull << 5) + 1;
const U32 G = 5;
const U32 MaxExp = 18;
const U32 MaxLen = 1u << MaxExp;

inline U32 ModAdd(U32 lhs, U32 rhs) NoExcept {
    U32 val = lhs + rhs;
    return val < Mod ? val : val - Mod;
}

inline U32 ModSub(U32 lhs, U32 rhs) NoExcept {
    U32 val = lhs - rhs;
    return val > lhs ? val + Mod : val;
}

inline U32 ModMul(U32 lhs, U32 rhs) NoExcept {
    return (U32) (((U64) lhs * rhs) % Mod);
}

inline U32 ModPow(U32 base, U32 exp) NoExcept {
    U32 res = 1;
    while (exp) {
        if (exp & 1)
            res = ModMul(res, base);
        base = ModMul(base, base);
        exp >>= 1;
    }
    return res;
}

inline U32 ModInv(U32 val) NoExcept {
    return ModPow(val, Mod - 2);
}

inline U32 ModDiv(U32 lhs, U32 rhs) NoExcept {
    return ModMul(lhs, ModInv(rhs));
}

U32 Log2[MaxLen];

inline void InitLog2() NoExcept {
    U32 log = 0;
    for (U32 i = 0; i < MaxLen; ++i) {
        Log2[i] = log;
        if (i == (1u << log))
            ++log;
    }
}

U32 NttInvLen[MaxExp];
U32* NttWFwd[MaxExp];
U32* NttWInv[MaxExp];
U32 NttWBuf[MaxLen << 4];

inline void ImplNtt(U32* A, U32 exp, U32* W) NoExcept {
    U32 len = 1u << exp;
    for (U32 i = 0, j = 0; i < len; ++i) {
        if (i > j)
            swap(A[i], A[j]);
        for (U32 k = len >> 1; (j ^= k) < k; k >>= 1);
    }
    for (U32 i = 0; i < exp; ++i) {
        U32 zChunk = 1u << i;
        for (U32 j = 0; j < len; j += 2u << i) {
            for (U32 k = 0; k < zChunk; ++k) {
                U32 t = ModMul(W[(len >> (i + 1)) * k], A[j + k + zChunk]);
                A[j + k + zChunk] = ModSub(A[j + k], t);
                A[j + k] = ModAdd(A[j + k], t);
            }
        }
    }
}

inline void Ntt(U32* A, U32 exp) NoExcept {
    ImplNtt(A, exp, NttWFwd[exp]);
}

inline void INtt(U32* A, U32 exp) NoExcept {
    ImplNtt(A, exp, NttWInv[exp]);
    U32 len = 1u << exp;
    for (U32 i = 0; i < len; ++i)
        A[i] = ModMul(A[i], NttInvLen[exp]);
}

inline void Ntt(const U32* A, U32 N, U32* B, U32 exp) {
    U32 len = 1u << exp;
    copy(A, A + N, B);
    fill(B + N, B + len, 0);
    Ntt(B, exp);
}

inline void InitNtt() NoExcept {
    U32* buf = NttWBuf;
    for (U32 exp = MaxExp; exp--; ) {
        U32 len = 1u << exp;
        U32* fwd = buf;
        U32* inv = buf + len;
        buf += len << 1;
        U32 wn = ModPow(G, (Mod - 1) >> exp);
        fwd[0] = 1;
        for (U32 i = 1; i < len; ++i)
            fwd[i] = ModMul(fwd[i - 1], wn);
        wn = ModInv(wn);
        inv[0] = 1;
        for (U32 i = 1; i < len; ++i)
            inv[i] = ModMul(inv[i - 1], wn);
        NttWFwd[exp] = fwd;
        NttWInv[exp] = inv;
        NttInvLen[exp] = ModInv(len);
    }
}

// A(x) B(x) \equiv 1 (\mod x^n)
// n-1  n-1
void PolyInv(const U32* A, U32* B, U32 N) NoExcept {
    static U32 TA[MaxLen], TB[MaxLen];
    if (N == 1) {
        B[0] = ModInv(A[0]);
        return;
    }
    U32 M = (N + 1) >> 1;
    PolyInv(A, B, M);
    U32 exp = Log2[N + M - 1];
    U32 len = 1u << exp;
    Ntt(A, N, TA, exp);
    Ntt(B, M, TB, exp);
    for (U32 i = 0; i < len; ++i)
        TB[i] = ModMul(ModSub(2, ModMul(TA[i], TB[i])), TB[i]);
    INtt(TB, exp);
    copy(TB + M, TB + N, B + M);
}

U32 PolyFNtt[MaxLen], PolyFRInvNtt[MaxLen], PolyN, PolyM, PolyR;
U32 Q[MaxLen];

// A(x) \equiv F(x) G(x) + H(x) (\mod x^{2m-3})
// 2m-4        m-1  m-3    m-2
// n = 2m-3 <= A(x)
//      m   <= F(x)
// r =  m-2 <= G(x)
//      m-1 <= H(x)
// AR(x) \equiv FR(x) GR(x) + x^{m-2} HR(x) (\mod x^{m-2})
// AR(x) \equiv FR(x) GR(x) (\mod x^{m-2})
// AR(x) FR^{-1}(x) \equiv GR(x) (\mod x^{m-2})
// H(x) \equiv A(x) - F(x) G(x) (\mod x^{2m-3})
inline void PolySetDivisor(const U32* F, U32 M) NoExcept {
    static U32 T[MaxLen];
    PolyM = M;
    PolyR = M - 2;
    PolyN = PolyM + PolyR - 1;
    U32 exp = Log2[PolyN];
    Ntt(F, M, PolyFNtt, exp);
    reverse_copy(F, F + PolyM, T);
    PolyInv(T, PolyFRInvNtt, PolyM);
    U32 len = 1u << exp;
    fill(PolyFRInvNtt + PolyM, PolyFRInvNtt + len, 0);
    Ntt(PolyFRInvNtt, exp);
}

// A(x) => H(x)
// 2m-4    m-2
inline void PolyMod(U32* A) NoExcept {
    static U32 T[MaxLen];
    U32 exp = Log2[PolyN];
    U32 len = 1u << exp;
    reverse_copy(A + PolyN - PolyR, A + PolyN, T); // T is AR(x)
    fill(T + PolyR, T + len, 0);
    Ntt(T, exp);
    for (U32 i = 0; i < len; ++i)
        T[i] = ModMul(T[i], PolyFRInvNtt[i]);
    INtt(T, exp);
    reverse(T, T + PolyR); // T is G(x)
    copy(T, T + PolyR, Q);
    fill(T + PolyR, T + len, 0);
    Ntt(T, exp);
    for (U32 i = 0; i < len; ++i)
        T[i] = ModMul(T[i], PolyFNtt[i]);
    INtt(T, exp); // T is F(x) G(x)
    for (U32 i = 0; i < PolyN; ++i)
        A[i] = ModSub(A[i], T[i]); // A is H(x)
}

#include <cstdio>

U32 ReadPoly(U32* A) NoExcept {
    U32 len;
    scanf("%u", &len);
    for (U32 i = 0; i < len; ++i)
        scanf("%u", &A[i]);
    return len;
}

void PrintPoly(const U32* A, U32 N, const char* s = "") NoExcept {
    fputs(s, stdout);
    for (U32 i = 0; i < N; ++i)
        printf(" %u", A[i]);
    putchar('\n');
}

int main() {
    InitLog2();
    InitNtt();
    static U32 A[MaxLen], B[MaxLen], C[MaxLen];
    if (true) {
        U32 NA = ReadPoly(A);
        U32 NB = ReadPoly(B);
        if (NA > NB * 2 - 3) {
            puts("fuck");
            return 1;
        }
        U32 len = 1u << Log2[NB * 2 - 1];
        PrintPoly(A, len, "      A:");
        PrintPoly(B, len, "      B:");
        PolySetDivisor(B, NB);
        PolyMod(A);
        PrintPoly(Q, len, "A div B:");
        PrintPoly(A, len, "A mod B:");
    }
    if (false) {
        U32 NA = ReadPoly(A);
        PolyInv(A, B, NA);
        U32 len = 1u << Log2[NA];
        PrintPoly(A, len, "   A:");
        PrintPoly(B, len, "AInv:");
    }
    if (false) {
        U32 NA = ReadPoly(A);
        U32 NB = ReadPoly(B);
        U32 N = NA + NB - 1;
        U32 exp = Log2[N];
        U32 len = 1u << exp;
        printf("exp=%u len=%u\n", exp, len);
        Ntt(A, exp);
        Ntt(B, exp);
        PrintPoly(A, len, " Ntt(A):");
        PrintPoly(B, len, " Ntt(B):");
        for (U32 i = 0; i < len; ++i)
            A[i] = ModMul(A[i], B[i]);
        PrintPoly(A, len, "Ntt(AB):");
        INtt(A, exp);
        PrintPoly(A, len, "     AB:");
    }
    return 0;
}

