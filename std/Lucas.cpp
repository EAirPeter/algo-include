#include <iostream>

using namespace std;

// Lucas

long long C(long long m, long long n, long long p) {
    if (n >= p)
        return C(m / p, n / p, p) * C(m % p, n % p, p) % p;
    long long m0 = 1, n0 = 1;
    for (long long i = 1, j = m; i <= n; ++i, --j) {
        m0 = m0 * i % p;
        n0 = n0 * j % p;
    }
    long long m1 = 1;
    for (long long t = p - 2; t; t >>= 1) {
        if (t & 1)
            m1 = m1 * m0 % p;
        m0 = m0 * m0 % p;
    }
    return m1 * n0 % p;
}

int main() {
    int tt;
    cin >> tt;
    while (tt--) {
        long long n, m, p;
        cin >> n >> m >> p;
        cout << C(m + n, n, p) << endl;
    }
    return 0;
}

