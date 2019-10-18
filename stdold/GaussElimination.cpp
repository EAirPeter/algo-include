#include <algorithm>
#include <cstdio>
#include <utility>

using namespace std;

inline long long gcd(long long a, long long b) {
    if (a < 0)
        a = -a;
    if (b < 0)
        b = -b;
    while (b) {
        long long c = a % b;
        a = b;
        b = c;
    }
    return a;
}

unsigned N, K;
long long A[10][10];

// GaussElimination

int main() {
    unsigned tt, nn = 0;
    scanf("%u", &tt);
    while (tt--) {
        scanf("%u", &N);
        for (unsigned i = 0; i < N; ++i) {
            long long d = 0;
            for (unsigned j = 0; j < N; ++j) {
                double w;
                scanf("%lf", &w);
                A[i][j] = (long long) (w * 100 + (w < 0 ? -0.5 : 0.5));
                if (i == j)
                    A[i][j] -= 100;
                d = gcd(d, A[i][j]);
            }
            if (d)
                for (unsigned j = 0; j < N; ++j)
                    A[i][j] /= d;
        }
        for (unsigned i = K = 0; i < N; ++i) {
            if (!A[K][i]) {
                unsigned u = K + 1;
                while (u < N && !A[u][i])
                    ++u;
                if (u == N)
                    continue;
                for (unsigned k = i; k < N; ++k)
                    swap(A[K][k], A[u][k]);
            }
            for (unsigned j = K + 1; j < N; ++j)
                if (A[j][i]) {
                    long long d = gcd(A[K][i], A[j][i]);
                    long long m1 = A[K][i] / d;
                    long long m2 = A[j][i] / d;
                    A[j][i] = 0;
                    for (unsigned k = i + 1; k < N; ++k)
                        A[j][k] = A[j][k] * m1 - A[K][k] * m2;
                    d = 0;
                    for (unsigned k = i + 1; k < N; ++k)
                        d = gcd(d, A[j][k]);
                    if (d)
                        for (unsigned k = i + 1; k < N; ++k)
                            A[j][k] /= d;
                }
            ++K;
        }
        printf("%d%c", (int) (K < N), "\n "[++nn % 5 && tt]);
    }
    return 0;
}

