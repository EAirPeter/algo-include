#include <algorithm>
#include <cstdio>
#include <functional>

using namespace std;

const int MAXN = 101;

// Prim

int N, M;
int H[MAXN][MAXN];
int F[MAXN];
bool V[MAXN];
int Q[MAXN];

int main() {
    scanf("%d", &N);
    for (int i = 1; i <= N; ++i)
        for (int j = 1; j <= N; ++j)
            scanf("%d", &H[i][j]);
    scanf("%d", &M);
    while (M--) {
        int a, b;
        scanf("%d%d", &a, &b);
        H[a][b] = H[b][a] = 0;
    }
    fill(F + 1, F + N + 1, 0x3f3f3f3f);
    F[1] = 0;
    Q[0] = 1;
    int *q = Q + 1;
    int p = 0;
    while (Q < q) {
        int u = *Q & 0xfff;
        pop_heap(Q, q--, greater<int>());
        if (V[u])
            continue;
        p += F[u];
        V[u] = true;
        for (int v = 1; v <= N; ++v)
            if (H[u][v] < F[v]) {
                *q = ((F[v] = H[u][v]) << 12) | v;
                push_heap(Q, ++q, greater<int>());
            }
    }
    printf("%d\n", p);
    return 0;
}

