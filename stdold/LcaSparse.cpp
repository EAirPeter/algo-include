#include <algorithm>
#include <cstdio>
#include <cstring>
#include <utility>

using namespace std;

int H[100001], I[200001], J[200001], C;

inline void adde(int u, int v) {
    I[C] = H[u];
    J[C] = v;
    H[u] = C++;
}

// LCA Sparse

struct qry {
    int u, v, f;
} T[200001];

int F[100001][18];
int W[100001];
int B[131073];

inline int lca(int u, int v) {
    if (W[v] > W[u])
        swap(u, v);
    for (int w, s = W[u] - W[v]; s; s ^= w) {
        w = s & -s;
        u = F[u][B[w]];
    }
    while (u != v) {
        int w = 17;
        while (w >= 0 && F[u][w] == F[v][w])
            --w;
        if (w < 0)
            return F[u][0];
        u = F[u][w];
        v = F[v][w];
    }
    return u;
}

inline bool operator <(const qry &l, const qry &r) {
    return W[l.f] != W[r.f] ? W[l.f] > W[r.f] : memcmp(&l, &r, sizeof(qry)) < 0;
}

int N, M;
bool V[100001];
int Q[100001];

int main() {
    for (int i = 0; i < 18; ++i)
        B[1 << i] = i;
    while (scanf("%d%d", &N, &M) == 2) {
        C = 1;
        memset(H, 0, sizeof(H));
        for (int i = 1; i < N; ++i) {
            int u, v;
            scanf("%d%d", &u, &v);
            adde(u, v);
            adde(v, u);
        }
        memset(F, 0, sizeof(F));
        memset(W, 0, sizeof(W));
        W[Q[0] = 1] = 1;
        for (int qf = 0, ql = 1; qf < ql; ++qf)
            for (int i = H[Q[qf]]; i; i = I[i])
                if (!W[J[i]]) {
                    W[Q[ql++] = J[i]] = W[Q[qf]] + 1;
                    F[J[i]][0] = Q[qf];
                    for (int j = 0; F[Q[qf]][j]; ++j)
                        F[J[i]][j + 1] = F[F[J[i]][j]][j];
                }
        for (int i = 0; i < M; ++i) {
            scanf("%d%d", &T[i].u, &T[i].v);
            T[i].f = lca(T[i].u, T[i].v);
        }
        sort(T, T + M);
        memset(V, 0, sizeof(V));
        int x = 0;
        for (int i = 0; i < M && T[i].f; ++i)
            if (!V[T[i].u] && !V[T[i].v]) {
                ++x;
                V[Q[0] = T[i].f] = true;
                for (int qf = 0, ql = 1; qf < ql; ++qf)
                    for (int j = H[Q[qf]]; j; j = I[j])
                        if (W[J[j]] > W[Q[qf]] && !V[J[j]])
                            V[Q[ql++] = J[j]] = true;
            }
        printf("%d\n", x);
    }
    return 0;
}

