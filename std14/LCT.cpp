// LGOJ3690
#define IBufLen 1u << 24
#define OBufLen 1u << 22

#include "std14/BinIo.hpp"

#include <cstdint>
#include <utility>

using namespace std;

constexpr size_t MaxN = 300001;

struct LcNode {
    LcNode* pa;
    LcNode* ch[2];
    uint32_t val;
    uint32_t sum;
    bool rev;
} nil{}; // Dummy node, for simplicity

inline void SpMod(LcNode* p, uint32_t val) {
    p->sum ^= p->val ^ val;
    p->val = val;
}

inline void SpRev(LcNode* p) {
    swap(p->ch[0], p->ch[1]);
    p->rev ^= 1;
}

inline void SpPush(LcNode* p) {
    if (p->rev) {
        p->rev = false;
        SpRev(p->ch[0]);
        SpRev(p->ch[1]);
    }
}

inline void SpPull(LcNode* p) {
    p->sum = p->ch[0]->sum ^ p->val ^ p->ch[1]->sum;
}

inline bool SpIsRoot(LcNode* p) {
    return p->pa->ch[0] != p && p->pa->ch[1] != p;
}

inline void SpLink(LcNode* p, LcNode* c, bool d) {
    p->ch[d] = c;
    c->pa = p;
}

void SpSplay(LcNode* p) {
    static LcNode* stk[MaxN];
    size_t cnt = 0;
    for (auto q = p; !SpIsRoot(q); q = q->pa)
        stk[cnt++] = q->pa;
    while (cnt--)
        SpPush(stk[cnt]);
    SpPush(p);
    while (!SpIsRoot(p)) {
        auto q = p->pa;
        auto d = q->ch[1] == p;
        if (!SpIsRoot(q)) {
            auto r = q->pa;
            auto e = r->ch[1] == q;
            p->pa = r->pa;
            if (!SpIsRoot(r)) {
                auto s = r->pa;
                s->ch[s->ch[1] == r] = p;
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
                SpLink(r, q->ch[!d], d);
                SpLink(q, r, !d);
                SpLink(q, p->ch[!d], d);
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
                SpLink(r, p->ch[d], !d);
                SpLink(q, p->ch[!d], d);
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
            p->pa = q->pa;
            SpLink(q, p->ch[!d], d);
            SpLink(p, q, !d);
        }
        SpPull(q);
        SpPull(p);
    }
}

// Make a path from root to p
inline void LcExpose(LcNode* p) {
    for (LcNode* q = &nil; p != &nil; q = p, p = p->pa) {
        SpSplay(p);
        p->ch[1] = q;
        SpPull(p);
    }
}

// Find the root of the tree containing p
inline LcNode* LcRoot(LcNode* p) {
    LcExpose(p);
    SpSplay(p);
    while (p->ch[0] != &nil) {
        SpPush(p);
        p = p->ch[0];
    }
    SpSplay(p);
    return p;
}

// Make p the root of the tree containing p
inline void LcEvert(LcNode* p) {
    LcExpose(p);
    SpSplay(p);
    SpRev(p);
}

// Link p and q if they do not belong to same tree
inline void LcLink(LcNode* p, LcNode* q) {
    LcEvert(q);
    if (LcRoot(p) != q)
        q->pa = p;
}

// Cut the edge (p, q) if exists
inline void LcCut(LcNode* p, LcNode* q) {
    LcEvert(p);
    LcExpose(q);
    SpSplay(q);
    if (q->ch[0] == p) {
        p->pa = &nil;
        q->ch[0] = &nil;
        SpPull(q);
    }
}

inline uint32_t LcQry(LcNode* p, LcNode* q) {
    LcEvert(p);
    LcExpose(q);
    SpSplay(q);
    return q->sum;
}

inline void LcMod(LcNode* p, uint32_t val) {
    LcExpose(p);
    SpSplay(p);
    SpMod(p, val);
}

LcNode T[MaxN];

int main() {
    uint32_t N, M;
    IoGet(N, M);
    for (uint32_t i = 1; i <= N; ++i) {
        IoGet(T[i].val);
        T[i].sum = T[i].val;
        T[i].pa = &nil;
        T[i].ch[0] = &nil;
        T[i].ch[1] = &nil;
    }
    while (M--) {
        uint32_t op, u, v;
        IoGet(op, u, v);
        switch (op) {
        case 0:
            IoPut(LcQry(&T[u], &T[v]), '\n');
            break;
        case 1:
            LcLink(&T[u], &T[v]);
            break;
        case 2:
            LcCut(&T[u], &T[v]);
            break;
        case 3:
            LcMod(&T[u], v);
            break;
        }
    }
    return 0;
}
