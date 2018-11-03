#include <algorithm>
#include <cstdio>
#include <cstring>

using namespace std;

int H[100001], I[200001], J[200001], C;

inline void adde(int u, int v) {
    I[C] = H[u];
    J[C] = v;
    H[u] = C++;
}

// LCA Tarjan

struct qry {
    int u, v, f;
    qry *p;
} T[200001], *G[100001];
int D;

inline void addq(int u, int v) {
    T[D].u = u;
    T[D].v = v;
    T[D].f = 0;
    T[D].p = G[u];
    G[u] = T + D++;
}

int P[100001];
int W[100001];

int ufs(int x) {
    return x == P[x] ? x : P[x] = ufs(P[x]);
}

void dfs(int u) {
    P[u] = u;
    for (int i = H[u]; i; i = I[i])
        if (!W[J[i]]) {
            W[J[i]] = W[u] + 1;
            dfs(J[i]);
            P[J[i]] = u;
        }
    for (qry *q = G[u]; q; q = q->p)
        if (W[q->v])
            q->f = ufs(q->v);
}

inline bool operator <(const qry &l, const qry &r) {
    return W[l.f] != W[r.f] ? W[l.f] > W[r.f] : memcmp(&l, &r, sizeof(qry)) < 0;
}

int N, M;
bool V[100001];
int Q[100001];

int main() {
    while (scanf("%d%d", &N, &M) == 2) {
        C = 1;
        memset(H, 0, sizeof(H));
        for (int i = 1; i < N; ++i) {
            int u, v;
            scanf("%d%d", &u, &v);
            adde(u, v);
            adde(v, u);
        }
        D = 0;
        memset(G, 0, sizeof(G));
        while (M--) {
            int u, v;
            scanf("%d%d", &u, &v);
            addq(u, v);
            addq(v, u);
        }
        memset(W, 0, sizeof(W));
        W[1] = 1;
        dfs(1);
        sort(T, T + D);
        memset(V, 0, sizeof(V));
        int x = 0;
        for (int i = 0; i < D && T[i].f; ++i)
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

