#include <algorithm>
#include <cstdio>
#include <cstring>
#include <functional>

using namespace std;

const int MAXN = 1001;
const int MAXM = 21000;

int C;
int H[MAXN], I[MAXM], J[MAXM], K[MAXM];
int F[MAXN];
bool V[MAXN];
long long Q[100000];

inline void adde(int a, int b, int c) {
    I[C] = H[a];
    J[C] = b;
    K[C] = c;
    H[a] = C++;
}

// Dijkstra

int main() {
    int N, M, T;
    while (scanf("%d%d%d", &N, &M, &T) == 3) {
        C = 1;
        memset(H, 0, sizeof(H));
        while (M--) {
            int a, b, c;
            scanf("%d%d%d", &a, &b, &c);
            adde(a, b, c);
        }
        scanf("%d", &M);
        while (M--) {
            int a;
            scanf("%d", &a);
            adde(0, a, 0);
        }
        memset(V, 0, sizeof(V));
        memset(F, 0x3f, sizeof(F));
        Q[0] = F[0] = 0;
        long long *q = Q + 1;
        while (q - Q) {
            int u = (int) (*Q & 0xffffffff);
            pop_heap(Q, q--, greater<long long>());
            if (V[u])
                continue;
            V[u] = true;
            for (int i = H[u]; i; i = I[i])
                if (F[u] + K[i] < F[J[i]]) {
                    *q = ((long long) (F[J[i]] = F[u] + K[i]) << 32LL) | J[i];
                    push_heap(Q, ++q, greater<long long>());
                }
        }
        printf("%d\n", V[T] ? F[T] : -1);
    }
    return 0;
}

