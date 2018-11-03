#include <algorithm>
#include <cmath>
#include <complex>
#include <utility>

using namespace std;

const long double PI = acos(-1.0L);

// FastFourierTransform
// N shall be 2^k, V: 1(DFT), -1(IDFT)
void fft(complex<long double> *A, const unsigned N, const int V) {
    for(unsigned i = 0; i < N; ++i) {
        unsigned r = 0;
        for(unsigned j = i, k = 1; k < N; j >>= 1, k <<= 1)
            r = (r << 1) | (j & 1);
        if(i < r)
            swap(A[i], A[r]);
    }
    for(unsigned h = 2; h <= N; h <<= 1) {
        long double th = V * PI * 2 / h;
        complex<long double> wn(cos(th), sin(th));
        for(unsigned i = 0; i < N; i += h) {
            complex<long double> w(1, 0);
            for(unsigned j = 0; j < (h >> 1); ++j) {
                complex<long double> t(w * A[i + j + h / 2]);
                A[i + j + h / 2] = A[i + j] - t;
                A[i + j] += t;
                w *= wn;
            }
        }
    }
    if(V < 0)
        for(unsigned i = 0; i < N; ++i)
            A[i] /= N;
}

int main() {
}

