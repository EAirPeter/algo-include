#include <cstdio>
#include <cstring>

using namespace std;

// SuffixArray

int N, M;
int T[110000];
int SA[110000];
int T1[110000];
int T2[110000];
int W[110000];
int H[110000];

int main() {
    // Suffix Array
    int *R = T1;
    int *SA_ = T2;
    memset(W, 0, sizeof(int) * (unsigned) M);
    for (int i = 0; i < N; ++i)
        ++W[R[i] = T[i]];
    for (int i = 1; i < M; ++i)
        W[i] += W[i - 1];
    for (int i = N - 1; i >= 0; --i)
        SA[--W[R[i]]] = i;
    for (int j = 1; j < N; j <<= 1) {
        int k = 0;
        for (int i = N - j; i < N; ++i)
            SA_[k++] = i;
        for (int i = 0; i < N; ++i)
            if (SA[i] >= j)
                SA_[k++] = SA[i] - j;
        memset(W, 0, sizeof(int) * (unsigned) M);
        for (int i = 0; i < N; ++i)
            ++W[R[SA_[i]]];
        for (int i = 1; i < M; ++i)
            W[i] += W[i - 1];
        for (int i = N - 1; i >= 0; --i)
            SA[--W[R[SA_[i]]]] = SA_[i];
        int *R_ = SA_;
        R_[SA[0]] = M = 0;
        R[N] = -1;
        for (int i = 1; i < N; ++i)
            R_[SA[i]] = R[SA[i]] == R[SA[i - 1]] &&
                R[SA[i] + j] == R[SA[i - 1] + j] ?
                M : ++M;
        ++M;
        SA_ = R;
        R = R_;
    }
    // Height
    for (int i = 0, k = 0; i < N; ++i) {
        k ? --k : 0;
        if (R[i])
            for (int j = SA[R[i] - 1]; T[i + k] == T[j + k]; ++k);
        H[R[i]] = k;
    }
    // Suffix Array: SA, Rank: R, Height: H
    return 0;
}

