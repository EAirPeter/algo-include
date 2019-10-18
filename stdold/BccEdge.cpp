#include <algorithm>
#include <cstdio>

using namespace std;

// BccEdge

struct ed {
    int v;
    ed *r, *p;
    bool t;
} T[1000], T_[1000], *H[1001];
int C, D;
int N, M;

int DFN[1001], LOW[1001];

int R[1001], U[1001];

int ufs(int x) {
    return x == U[x] ? x : U[x] = ufs(U[x]);
}

void dfs(ed *f) {
    int u = f->v;
    DFN[u] = LOW[u] = ++D;
    for (ed *e = H[u]; e; e = e->p)
        if (!DFN[e->v]) {
            dfs(e);
            LOW[u] = min(LOW[u], LOW[e->v]);
            if (LOW[e->v] > DFN[u])
                e->t = e->r->t = true;
        }
        else if (e->r != f)
            LOW[u] = min(LOW[u], DFN[e->v]);
}

int main() {
    scanf("%d%d", &N, &M);
    for (int i = 0; i < M; ++i) {
        int u, v;
        scanf("%d%d", &u, &v);
        T[C].p = H[u];
        H[u] = T + C;
        H[u]->v = v;
        T_[C].p = H[v];
        H[v] = T_ + C;
        H[v]->v = u;
        H[u]->r = H[v];
        H[v]->r = H[u];
        ++C;
    }
    ed e = {1, NULL, NULL, false};
    dfs(&e);
    for (int i = 1; i <= N; ++i)
        U[i] = i;
    for (int i = 0; i < C; ++i)
        if (!T[i].t && ufs(T[i].v) != ufs(T_[i].v))
            U[U[T[i].v]] = U[T_[i].v];
    int x = 0;
    for (int i = 0; i < C; ++i)
        if (T[i].t) {
            if (!R[ufs(T[i].v)])
                ++x;
            else if (R[U[T[i].v]] == 1)
                --x;
            if (!R[ufs(T_[i].v)])
                ++x;
            else if (R[U[T_[i].v]] == 1)
                --x;
            ++R[U[T[i].v]];
            ++R[U[T_[i].v]];
        }
    printf("%d\n", (x + 1) >> 1);
    return 0;
}

