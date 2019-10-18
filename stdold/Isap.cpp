#include <cstdio>
#include <cstring>

using namespace std;

const int MAXN = 2010;
const int MAXM = 800000;

// ISAP

struct edge {
    int t, f;
    edge *p, *r;
} T[MAXM], *H[MAXN], *I[MAXN], *P[MAXN];

int C;
int G[MAXN], D[MAXN], L[MAXN];


inline void adde(const int a, const int b, const int c) {
    edge *e1 = T + C++;
    edge *e2 = T + C++;
    e1->t = b;
    e1->f = c;
    e1->p = H[a];
    e1->r = e2;
    H[a] = e1;
    e2->t = a;
    e2->f = 0;
    e2->p = H[b];
    e2->r = e1;
    H[b] = e2;
}

inline int isap(const int cnt, const int src, const int snk) {
    int maxf = 0;
    int curf = 0x3f3f3f3f;
    int u = src, v;
    edge *e;
    memcpy(I, H, sizeof(H));
    memset(G, 0, sizeof(G));
    memset(D, 0, sizeof(D));
    G[0] = cnt;
    while (D[src] < cnt) {
        L[u] = curf;
        for (e = I[u]; e; e = e->p) {
            v = e->t;
            if (e->f > 0 && D[u] == D[v] + 1) {
                I[u] = P[v] = e;
                if (e->f < curf)
                    curf = e->f;
                u = v;
                if (u == snk) {
                    maxf += curf;
                    while (u != src) {
                        P[u]->f -= curf;
                        P[u]->r->f += curf;
                        u = P[u]->r->t;
                    }
                    curf = 0x3f3f3f3f;
                }
                break;
            }
        }
        if (e)
            continue;
        if (!--G[D[u]])
            break;
        int mind = cnt - 1;
        for (e = H[u]; e; e = e->p)
            if (e->f > 0 && D[e->t] < mind) {
                I[u] = e;
                mind = D[e->t];
            }
        ++G[D[u] = mind + 1];
        if (u != src) {
            u = P[u]->r->t;
            curf = L[u];
        }
    }
    return maxf;
}

int N, M;
bool V[500];

int main() {
    int tt, a, b, c, ss, cnt;
    scanf("%d", &tt);
    for (int tc = 1; tc <= tt; ++tc) {
        memset(V, false, sizeof(V));
        memset(H, 0, sizeof(H));
        ss = 0;
        C = 0;
        scanf("%d%d", &N, &M);
        cnt = N + 2;
        for (int i = 0; i < N; ++i) {
            scanf("%d%d%d", &a, &b, &c);
            adde(2004, i, a);
            for (int j = b - 1; j < c; ++j) {
                adde(i, j + 1000, 1);
                if (!V[j]) {
                    adde(j + 1000, 2005, M);
                    V[j] = true;
                    ++cnt;
                }
            }
            ss += a;
        }
        if (isap(cnt, 2004, 2005) == ss)
            printf("Case %d: Yes\n\n", tc);
        else
            printf("Case %d: No\n\n", tc);
    }
    return 0;
}

