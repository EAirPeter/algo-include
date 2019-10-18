// LGOJ3690
#define IBufLen 1u << 24
#define OBufLen 1u << 22

#include "std14/BinIo.hpp"

#include <cstdint>
#include <utility>

using namespace std;

constexpr size_t MaxN = 300001;

using NIdx = uint32_t;

struct LcNode {
    NIdx pa;
    NIdx ch[2];
    uint32_t val;
    uint32_t sum;
    bool rev;
} T[MaxN];

inline void SpMod(NIdx p, uint32_t val) {
    T[p].sum ^= T[p].val ^ val;
    T[p].val = val;
}

inline void SpRev(NIdx p) {
    swap(T[p].ch[0], T[p].ch[1]);
    T[p].rev ^= 1;
}

inline void SpPush(NIdx p) {
    if (T[p].rev) {
        T[p].rev = false;
        SpRev(T[p].ch[0]);
        SpRev(T[p].ch[1]);
    }
}

inline void SpPull(NIdx p) {
    T[p].sum = T[T[p].ch[0]].sum ^ T[p].val ^ T[T[p].ch[1]].sum;
}

inline bool SpIsRoot(NIdx p) {
    return T[T[p].pa].ch[0] != p && T[T[p].pa].ch[1] != p;
}

inline void SpLink(NIdx p, NIdx c, bool d) {
    T[p].ch[d] = c;
    T[c].pa = p;
}

void SpSplay(NIdx p) {
    static NIdx stk[MaxN];
    size_t cnt = 0;
    for (auto q = p; !SpIsRoot(q); q = T[q].pa)
        stk[cnt++] = T[q].pa;
    while (cnt--)
        SpPush(stk[cnt]);
    SpPush(p);
    while (!SpIsRoot(p)) {
        auto q = T[p].pa;
        auto d = T[q].ch[1] == p;
        if (!SpIsRoot(q)) {
            auto r = T[q].pa;
            auto e = T[r].ch[1] == q;
            T[p].pa = T[r].pa;
            if (!SpIsRoot(r)) {
                auto s = T[r].pa;
                T[s].ch[T[s].ch[1] == r] = p;
            }
            if (d == e) {
                // ZigZig
                //       s          s
                //       |          |
                //       r          p
                //      / \        / \
                //     q  r1  =>  p0  q
                //    / \            / \
                //   p  q1          p1  r
                //  / \                / \
                // p0 p1              q1 r1
                SpLink(r, T[q].ch[!d], d);
                SpLink(q, r, !d);
                SpLink(q, T[p].ch[!d], d);
                SpLink(p, q, !d);
            }
            else {
                // ZigZag
                //   s               s
                //   |               |
                //   r               p
                //  / \             / \
                // r0  q    =>     /   \
                //    / \         r     q
                //   p  q1       / \   / \
                //  / \         r0 p0 p1 q1
                // p0 p1
                SpLink(r, T[p].ch[d], !d);
                SpLink(q, T[p].ch[!d], d);
                SpLink(p, r, d);
                SpLink(p, q, !d);
            }
            SpPull(r);
        }
        else {
            // Zig
            //      |dot       |dot
            //      q          p
            //     / \        / \
            //    p  q1  =>  p0  q
            //   / \            / \
            //  p0 p1          p1 q1
            T[p].pa = T[q].pa;
            SpLink(q, T[p].ch[!d], d);
            SpLink(p, q, !d);
        }
        SpPull(q);
        SpPull(p);
    }
}

inline void LcExpose(NIdx p) {
    for (NIdx q = 0; p; q = p, p = T[p].pa) {
        SpSplay(p);
        T[p].ch[1] = q;
        SpPull(p);
    }
}

inline NIdx LcRoot(NIdx p) {
    LcExpose(p);
    SpSplay(p);
    while (T[p].ch[0]) {
        SpPush(p);
        p = T[p].ch[0];
    }
    SpSplay(p);
    return p;
}

inline void LcEvert(NIdx p) {
    LcExpose(p);
    SpSplay(p);
    SpRev(p);
}

inline void LcLink(NIdx p, NIdx q) {
    LcEvert(q);
    if (LcRoot(p) != q)
        T[q].pa = p;
}

inline void LcCut(NIdx p, NIdx q) {
    LcEvert(p);
    LcExpose(q);
    SpSplay(q);
    if (T[q].ch[0] == p) {
        T[p].pa = 0;
        T[q].ch[0] = 0;
        SpPull(q);
    }
}

inline uint32_t LcQry(NIdx p, NIdx q) {
    LcEvert(p);
    LcExpose(q);
    SpSplay(q);
    return T[q].sum;
}

inline void LcMod(NIdx p, uint32_t val) {
    LcExpose(p);
    SpSplay(p);
    SpMod(p, val);
}

int main() {
    uint32_t N, M;
    IoGet(N, M);
    for (uint32_t i = 1; i <= N; ++i) {
        IoGet(T[i].val);
        T[i].sum = T[i].val;
    }
    while (M--) {
        uint32_t op, u, v;
        IoGet(op, u, v);
        switch (op) {
        case 0:
            IoPut(LcQry(u, v), '\n');
            break;
        case 1:
            LcLink(u, v);
            break;
        case 2:
            LcCut(u, v);
            break;
        case 3:
            LcMod(u, v);
            break;
        }
    }
    return 0;
}
