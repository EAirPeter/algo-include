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
    size_t idxHeap;
    Dis dist;
} GVtcs[MaxN];

struct GEdge {
    GEdge* next;
    GVertex* vertex;
    Dis weight;
} GEdgs[MaxM];

size_t GCEdg;

GVertex* GDijHeap[MaxN];
size_t GZDijHeap;

inline void GDijHeapDecKey(GVertex* U) noexcept {
    auto idxU = U->idxHeap ? U->idxHeap : ++GZDijHeap;
    while (idxU > 1) {
        auto idxV = idxU >> 1;
        auto V = GDijHeap[idxV];
        if (U->dist >= V->dist)
            break;
        GDijHeap[idxU] = V;
        V->idxHeap = idxU;
        idxU = idxV;
    }
    GDijHeap[idxU] = U;
    U->idxHeap = idxU;
}

inline GVertex* GDijHeapExtract() noexcept {
    auto res = GDijHeap[1];
    auto idxU = size_t{1};
    auto U = GDijHeap[GZDijHeap--];
    while ((idxU << 1) <= GZDijHeap) {
        auto idxV = idxU << 1;
        auto idxW = idxV + 1;
        auto V = GDijHeap[idxV];
        auto W = GDijHeap[idxW];
        if (U->dist <= V->dist && U->dist <= W->dist)
            break;
        if (V->dist > W->dist) {
            idxV = idxW;
            V = W;
        }
        GDijHeap[idxU] = V;
        V->idxHeap = idxU;
        idxU = idxV;
    }
    GDijHeap[idxU] = U;
    U->idxHeap = idxU;
    return res;
}

inline void GAddEdge(GVertex* U, GVertex *V, Dis w) noexcept {
    auto e = GEdgs + GCEdg++;
    e->next = U->edge;
    e->vertex = V;
    e->weight = w;
    U->edge = e;
}

inline void GDijkstra(GVertex* S) noexcept {
    GDijHeapDecKey(S);
    while (GZDijHeap) {
        auto U = GDijHeapExtract();
        for (auto e = U->edge; e; e = e->next) {
            auto V = e->vertex;
            if (!V->idxHeap || U->dist + e->weight < V->dist) {
                V->dist = U->dist + e->weight;
                GDijHeapDecKey(V);
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
    GDijkstra(&GVtcs[S]);
    for (size_t i = 1; i < N; ++i)
        printf("%zu ", GVtcs[i].dist);
    printf("%zu\n", GVtcs[N].dist);
    return 0;
}
