// LGOJ4779
#include "Heap.hpp"

#include <algorithm>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <utility>

using namespace std;

constexpr size_t MaxN = 100001;
constexpr size_t MaxM = 200001;

using Dis = size_t;

struct GVertex;
struct GEdge;

struct GVertex {
    size_t edge;
    Dis dist;
    bool vis;
} GVtcs[MaxN];

struct GEdge {
    size_t next;
    size_t vertex;
    Dis weight;
} GEdgs[MaxM];

size_t GCEdg;

inline void GAddEdge(size_t u, size_t v, Dis w) noexcept {
    auto e = ++GCEdg;
    auto& E = GEdgs[e];
    auto& U = GVtcs[u];
    E.next = U.edge;
    E.vertex = v;
    E.weight = w;
    U.edge = e;
}

inline void GDijkstra(size_t s) noexcept {
    static KeyedHeap<size_t, Dis, MaxN> H;
    auto&S = GVtcs[s];
    S.vis = true;
    H.Clear();
    H.Push(s, GVtcs[s].dist);
    while (!H.IsEmpty()) {
        auto u = H.Extract();
        auto& U = GVtcs[u];
        for (auto e = U.edge; e; e = GEdgs[e].next) {
            auto& E = GEdgs[e];
            auto& V = GVtcs[E.vertex];
            if (!V.vis || U.dist + E.weight < V.dist) {
                V.dist = U.dist + E.weight;
                V.vis = true;
                H.DecKey(E.vertex, V.dist);
            }
        }
    }
}

int main() {
    size_t N, M, s;
    scanf("%zu%zu%zu", &N, &M, &s);
    for (size_t i = 0; i < M; ++i) {
        size_t u, v, w;
        scanf("%zu%zu%zu", &u, &v, &w);
        GAddEdge(u, v, w);
    }
    GDijkstra(s);
    for (size_t i = 1; i < N; ++i)
        printf("%zu ", GVtcs[i].dist);
    printf("%zu\n", GVtcs[N].dist);
    return 0;
}
