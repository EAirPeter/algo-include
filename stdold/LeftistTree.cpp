#include <algorithm>
#include <cstdio>
#include <cstring>
#include <utility>

using namespace std;

// LeftistTree

struct hnode {
    int d, v;
    hnode *l, *r;
} T[200001], *H[100001];
int C;

hnode *hnew(int v) {
    hnode *h = T + C++;
    h->d = 0;
    h->v = v;
    h->l = h->r = NULL;
    return h;
}

hnode *hmrg(hnode *l, hnode *r) {
    if (!l)
        return r;
    if (!r)
        return l;
    if (l->v > r->v)
        swap(l, r);
    l->r = hmrg(l->r, r);
    if (!l->l || (l->r && l->l->d < l->r->d))
        swap(l->l, l->r);
    l->d = l->r ? l->r->d + 1 : 0;
    return l;
}

struct qry {
    int x, y;
    inline bool operator <(const qry &r) const {
        return y != r.y ? y < r.y : x < r.x;
    }
} E[200001];
int D;

int N, M, X;
int LW[100001], RW[100001], L[100001], R[100001];
int A[100001], B[100001];

int U[100001];

int ufs(int x) {
    return x == U[x] ? x : U[x] = ufs(U[x]);
}

int main() {
    int tt;
    scanf("%d", &tt);
    for (int nn = 1; nn <= tt; ++nn) {
        scanf("%d%d", &N, &M);
        for (int i = 1; i < N; ++i) {
            int w;
            scanf("%d", &w);
            LW[i + 1] = RW[i] = w;
            L[i] = i - 1;
            R[i] = i + 1;
            U[i] = i;
            H[i] = NULL;
        }
        L[N] = N - 1;
        U[N] = N;
        H[N] = NULL;
        LW[1] = 0x7fffffff;
        RW[N] = 0x7fffffff;
        C = D = X = 0;
        while (M--) {
            int x, y, z;
            scanf("%d%d%d", &x, &y, &z);
            if (z) {
                E[D].x = x;
                E[D].y = y + 1;
                ++D;
            }
            else {
                H[x] = hmrg(H[x], hnew(y));
                ++X;
            }
        }
        sort(E, E + D);
        memset(A, 0, sizeof(A));
        memset(B, 0, sizeof(B));
        for (int i = 0; i < D; ++i) {
            int x = ufs(E[i].x);
            for (int v, u = x; E[i].y > LW[u]; ) {
                v = ufs(L[u]);
                R[ufs(L[v])] = u;
                L[u] = U[L[v]];
                LW[u] = LW[v];
                U[v] = u;
                H[u] = hmrg(H[u], H[v]);
                A[u] += A[v];
                B[u] += B[v];
            }
            for (int v, u = x; E[i].y > RW[u]; ) {
                v = ufs(R[u]);
                L[ufs(R[v])] = u;
                R[u] = U[R[v]];
                RW[u] = RW[v];
                U[v] = u;
                H[u] = hmrg(H[u], H[v]);
                A[u] += A[v];
                B[u] += B[v];
            }
            while (H[x] && E[i].y > H[x]->v) {
                H[x] = hmrg(H[x]->l, H[x]->r);
                ++A[x];
            }
            if (++B[x] > A[x]) {
                X += B[x] - A[x];
                A[x] = B[x] = 0;
            }
        }
        printf("Case #%d: %d\n", nn, X);
    }
    return 0;
}

