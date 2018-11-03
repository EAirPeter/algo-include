#include <algorithm>
#include <cstdio>
#include <cstring>

using namespace std;

// BccVertex

struct ed {
    int v;
    ed *p;
} T[2000000], *H[1001];
int C, N, M;

bool Adj[1001][1001];
bool W[1001];

int V[1001], Q[1001], R;
int DFN[1001], LOW[1001], D;
bool CV[1001];
int CT[1001], CQ[1001];

void dfs(int u, int p) {
    V[Q[R++] = u] = true;
    LOW[u] = DFN[u] = ++D;
    for (ed *e = H[u]; e; e = e->p)
        if (e->v != p) {
            if (!DFN[e->v]) {
                dfs(e->v, u);
                LOW[u] = min(LOW[u], LOW[e->v]);
                if (LOW[e->v] >= DFN[u]) {
                    // CV[v] if v is in the current BCC
                    memset(CV, 0, sizeof(CV));
                    int r = R;
                    do {
                        V[Q[--R]] = false;
                        CV[Q[R]] = true;
                    } while (Q[R] != e->v);
                    CV[u] = true;
                    // Check Bipart Graph
                    memset(CT, 0, sizeof(CT));
                    CT[CQ[0] = u] = 2;
                    bool f = true;
                    for (int qf = 0, ql = 1; f && qf < ql; ++qf)
                        for (ed *e = H[CQ[qf]]; f && e; e = e->p)
                            if (CV[e->v]) {
                                if (CT[e->v])
                                    f = CT[e->v] != CT[CQ[qf]];
                                else {
                                    CT[e->v] = CT[CQ[qf]] ^ 1;
                                    CQ[ql++] = e->v;
                                }
                            }
                    if (!f) {
                        W[u] = true;
                        for (int i = R; i < r; ++i)
                            W[Q[i]] = true;
                    }
                }
            }
            else if (V[e->v])
                LOW[u] = min(LOW[u], DFN[e->v]);
        }
}

int main() {
    for (;;) {
        scanf("%d%d", &N, &M);
        if (!N && !M)
            break;
        memset(Adj, 0, sizeof(Adj));
        while (M--) {
            int u, v;
            scanf("%d%d", &u, &v);
            Adj[u][v] = Adj[v][u] = true;
        }
        C = 0;
        memset(H, 0, sizeof(H));
        for (int u = 1; u <= N; ++u)
            for (int v = 1; v <= N; ++v)
                if (u != v && !Adj[u][v]) {
                    T[C].v = v;
                    T[C].p = H[u];
                    H[u] = T + C++;
                }
        D = 0;
        R = 0;
        memset(V, 0, sizeof(V));
        memset(W, 0, sizeof(W));
        memset(DFN, 0, sizeof(DFN));
        for (int i = 1; i <= N; ++i)
            if (!DFN[i])
                dfs(i, -1);
        int x = 0;
        for (int i = 1; i <= N; ++i)
            if (!W[i])
                ++x;
        printf("%d\n", x);
    }
    return 0;
}

