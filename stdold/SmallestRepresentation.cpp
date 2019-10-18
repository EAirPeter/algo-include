#include <algorithm>
#include <cstdio>
#include <cstring>

using namespace std;

// SmallestRepresentation

char S[200001];

int main() {
    int tt;
    scanf("%d", &tt);
    while (tt--) {
        int N;
        scanf("%d%s", &N, S);
        memcpy(S + N, S, (unsigned) N);
        int w = 0;
        for (int i = 1; i < N; ) {
            int j = 0;
            while (j < N && S[w + j] == S[i + j])
                ++j;
            if (j == N)
                break;
            if (S[w + j] < S[i + j])
                i += j + 1;
            else {
                int t = w + j;
                w = i;
                i = max(t, i) + 1;
            }
        }
        printf("%d\n", w);
    }
    return 0;
}

