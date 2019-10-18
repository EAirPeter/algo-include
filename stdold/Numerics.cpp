#include <cstdio>

using namespace std;

const unsigned MDN = 1000000007;
const unsigned MAXN = 100000;

// Numerics

bool Vis[MAXN];
unsigned Pri[MAXN], CPri;
unsigned Phi[MAXN];
unsigned Inv[MAXN];
 
inline unsigned pwr(unsigned m, unsigned n) {
    unsigned x = 1;
    while (n) {
        if (n & 1)
            x = (unsigned) (((unsigned long long) x * m) % MDN);
        m = (unsigned) (((unsigned long long) m * m) % MDN);
        n >>= 1;
    }
    return x;
}

inline unsigned gcd(unsigned a, unsigned b) {
    while (b) {
        unsigned c = a % b;
        a = b;
        b = c;
    }
    return a;
}

unsigned exgcd(unsigned a, unsigned b, int &x, int &y) {
    if (!b) {
        x = 1;
        y = 0;
        return a;
    }
    unsigned d = exgcd(b, a % b, y, x);
    y -= x * (int) (a / b);
    return d;
}

inline unsigned inv(unsigned n, unsigned m) {
    int x, y;
    exgcd(n, m, x, y);
    return ((unsigned) (x % (int) m) + m) % m;
}

inline void primes() {
    for (unsigned i = 2; i < MAXN; ++i) {
        if (!Vis[i])
            Pri[CPri++] = i;
        for (unsigned j = 0; j < CPri && i * Pri[j] < MAXN; ++j) {
            Vis[i * Pri[j]] = true;
            if (!(i % Pri[j]))
                break;
        }
    }
}

inline void phis() {
    Phi[1] = 1;
    for (unsigned i = 2; i < MAXN; ++i) {
        if (!Vis[i]) {
            Pri[CPri++] = i;
            Phi[i] = i - 1;
        }
        for (unsigned j = 0; j < CPri && i * Pri[j] < MAXN; ++j) {
            Vis[i * Pri[j]] = true;
            if (i % Pri[j])
                Phi[i * Pri[j]] = Phi[i] * (Pri[j] - 1);
            else {
                Phi[i * Pri[j]] = Phi[i] * Pri[j];
                break;
            }
        }
    }
}

inline void invs() {
    Inv[1] = 1;
    for (unsigned i = 2; i < MAXN; ++i)
        Inv[i] = (unsigned)
            ((unsigned long long) (MDN - MDN / i) * Inv[MDN % i]) % MDN;
}

// Polya: SUM(N^Ci) / card(C)
// Cube:
//   {l}{u}{r}{d}{f}{b} x1 x1   n^6
//   {f}{b}{ldru}       x2 x3  6n^3
//   {f}{b}{lr}{du}     x1 x3  3n^4
//   {fru}{dbl}         x2 x4  8n^2
//   {fr}{ud}{bl}       x2 x3  6n^3
//                      card = 1+6+3+8+6=24

int main() {
}

