#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

const int MAXN = 203;
const int MAXM = 60000;

// MinimumCostMaximumFlow

struct ed {
    int v, f, c;
    ed *p, *r;
} T[MAXM], *H[MAXN], *P[MAXN];
int C;

inline void adde(int u, int v, int f, int c) {
    ed *uv = T + C++;
    ed *vu = T + C++;
    uv->v = v;
    vu->v = u;
    uv->f = f;
    vu->f = 0;
    uv->c = c;
    vu->c = -c;
    uv->p = H[u];
    vu->p = H[v];
    uv->r = vu;
    vu->r = uv;
    H[u] = uv;
    H[v] = vu;
}

int Q[100000], F[MAXN];
bool V[MAXN];

bool bfs(int src, int dst) {
    memset(F, 0x3f, sizeof(F));
    P[src] = NULL;
    F[src] = 0;
    V[Q[0] = src] = true;
    for (int qf = 0, ql = 1; qf < ql; ++qf) {
        for (ed *e = H[Q[qf]]; e; e = e->p)
            if (e->f && F[Q[qf]] + e->c < F[e->v]) {
                F[e->v] = F[Q[qf]] + e->c;
                P[e->v] = e;
                if (!V[e->v])
                    V[Q[ql++] = e->v] = true;
            }
        V[Q[qf]] = false;
    }
    return F[dst] != 0x3f3f3f3f;
}

int N, M, K;
char S[10][11];

int main() {
    int tt;
    scanf("%d", &tt);
    for (int nn = 1; nn <= tt; ++nn) {
        scanf("%d%d%d", &N, &M, &K);
        for (int i = 0; i < N; ++i)
            scanf("%s", S[i]);
        C = 0;
        memset(H, 0, sizeof(H));
        adde(201, 200, K, 0);
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < M; ++j) {
                int w = (i * M + j) << 1;
                adde(201, w, 1, 0);
                adde(200, w | 1, 1, 0);
                adde(w | 1, 202, 1, 0);
                for (int k = i; k < N; ++k)
                    for (int l = j; l < M; ++l)
                        if (i != k || j != l)
                            adde(w, ((k * M + l) << 1) | 1, 1,
                                abs(i - k) + abs(j - l) - 1 -
                                (S[i][j] == S[k][l] ? S[i][j] & 0x0f : 0));
            }
        int c = 0;
        int x = 0;
        // mcmf
        while (bfs(201, 202)) {
            int f = 0x3f3f3f3f;
            for (ed *e = P[202]; e; e = P[e->r->v])
                if (e->f < f)
                    f = e->f;
            for (ed *e = P[202]; e; e = P[e->r->v]) {
                e->f -= f;
                e->r->f += f;
            }
            c += f * F[202];
            x += f;
        }
        printf("Case %d : %d\n", nn, x == M * N ? -c : -1);
    }
    return 0;
}

