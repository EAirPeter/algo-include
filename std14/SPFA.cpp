// LGOJ4779
#include <algorithm>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <utility>

using namespace std;

constexpr size_t MaxN = 100001;
constexpr size_t MaxM = 200000;

using Dis = size_t;

struct GVertex;
struct GEdge;

struct GVertex {
    GEdge* edge;
    GVertex* parent;
    GVertex* child;
    GVertex* tprev;
    GVertex* tnext;
    GVertex* qprev;
    GVertex* qnext;
    Dis dist;
    bool inQ;
} GVtcs[MaxN];

struct GEdge {
    GEdge* next;
    GVertex* vertex;
    Dis weight;
} GEdgs[MaxM];

size_t GCEdg;

inline void GAddEdge(GVertex* U, GVertex *V, Dis w) noexcept {
    auto e = GEdgs + GCEdg++;
    e->next = U->edge;
    e->vertex = V;
    e->weight = w;
    U->edge = e;
}

inline void GTLink(GVertex* U, GVertex* V) noexcept {
    U->tnext = V;
    V->tprev = U;
}

inline void GQLink(GVertex* U, GVertex* V) noexcept {
    U->qnext = V;
    V->qprev = U;
}

void GSpfaClearInQ(GVertex* U) noexcept {
    if (!U)
        return;
    auto V = U;
    do {
        if (V->qnext) {
            GQLink(V->qprev, V->qnext);
            V->qnext = nullptr;
        }
        GSpfaClearInQ(V->child);
        V = V->tnext;
    }
    while (V != U);
}

inline void GSpfa(GVertex* S) noexcept {
    GVertex queue;
    auto Q = &queue;
    S->parent = S;
    GQLink(Q, S);
    GQLink(S, Q);
    while (Q->qnext != Q) {
        auto U = Q->qnext;
        GQLink(Q, U->qnext);
        U->qnext = nullptr;
        for (auto e = U->edge; e; e = e->next) {
            auto V = e->vertex;
            if (!V->parent || U->dist + e->weight < V->dist) {
                /* Negative Cycle Detection
                if (V->parent) {
                    for (auto W = U; W->parent != W; W = W->parent)
                        if (W == V)
                            return false;
                } */
                GSpfaClearInQ(V->child);
                GTLink(V->tprev, V->tnext);
                if (V->parent && V->parent->child == V)
                    V->parent->child = V->tnext == V ? nullptr : V->tnext;
                V->parent = U;
                if (U->child) {
                    GTLink(V, U->child->tnext);
                    GTLink(U->child, V);
                }
                else {
                    GTLink(V, V);
                    U->child = V;
                }
                V->dist = U->dist + e->weight;
                if (V->qnext)
                    GQLink(V->qprev, V->qnext);
                GQLink(Q->qprev, V);
                GQLink(V, Q);
            }
        }
    }
}

int main() {
    size_t N, M, S;
    scanf("%zu%zu%zu", &N, &M, &S);
    for (size_t i = 0; i < M; ++i) {
        size_t u, v, w;
        scanf("%zu%zu%zu", &u, &v, &w);
        GAddEdge(&GVtcs[u], &GVtcs[v], w);
    }
    for (size_t i = 1; i <= N; ++i) {
        auto U = &GVtcs[i];
        GTLink(U, U);
    }
    GSpfa(&GVtcs[S]);
    for (size_t i = 1; i < N; ++i)
        printf("%zu ", GVtcs[i].dist);
    printf("%zu\n", GVtcs[N].dist);
    return 0;
}
