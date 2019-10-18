// LGOJ3690
#define IBufLen 1u << 24
#define OBufLen 1u << 22

#include "std14/StdIo.hpp"

#include <utility>

using namespace std;

constexpr size_t MaxN = 300001;

struct LcNode {
    LcNode* pa;
    LcNode* ch[2];
    uint32_t val;
    uint32_t sum;
    bool rev;
};

inline uint32_t SpSum(LcNode* p) {
    return p ? p->sum : 0;
}

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
        if (p->ch[0])
            SpRev(p->ch[0]);
        if (p->ch[1])
            SpRev(p->ch[1]);
    }
}

inline void SpPull(LcNode* p) {
    p->sum = SpSum(p->ch[0]) ^ p->val ^ SpSum(p->ch[1]);
}

inline bool SpIsRoot(LcNode* p) {
    return !p->pa || (p->pa->ch[0] != p && p->pa->ch[1] != p);
}

inline void SpLink(LcNode* p, LcNode* c, bool d) {
    p->ch[d] = c;
    c->pa = p;
}

/*inline void SpRot(LcNode* p, bool d) {
    auto q = p->pa;
    q->ch[d] = p->ch[d ^ 1];
    p->ch[d ^ 1] = q;
    if (auto r = q->pa)
        r->ch[r->ch[1] == q] = p;
    p->pa = q->pa;
    q->pa = p;
    q->ch[d]->pa = q;
}*/

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
        if (auto r = q->pa) {
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
                SpLink(r, q->ch[d ^ 1], d);
                SpLink(q, r, d ^ 1);
                SpLink(q, p->ch[d ^ 1], d);
                SpLink(p, q, d ^ 1);
                // SpRot(q, e);
                // SpRot(p, d);
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
                SpLink(r, p->ch[d], d ^ 1);
                SpLink(q, p->ch[d ^ 1], d);
                SpLink(p, r, d);
                SpLink(p, q, d ^ 1);
                // SpRot(p, d);
                // SpRot(p, e);
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
            SpLink(q, p->ch[d ^ 1], d);
            SpLink(p, q, d ^ 1);
            // SpRot(p, d);
        }
        SpPull(q);
        SpPull(p);
    }
}

inline void LcExpose(LcNode* p) {
    for (LcNode* q = nullptr; p; q = p, p = p->pa) {
        SpSplay(p);
        p->ch[1] = q;
        SpPull(p);
    }
}

inline LcNode* LcRoot(LcNode* p) {
    LcExpose(p);
    SpSplay(p);
    while (p->ch[0]) {
        SpPush(p);
        p = p->ch[0];
    }
    return p;
}

inline void LcEvert(LcNode* p) {
    LcExpose(p);
    SpSplay(p);
    SpRev(p);
}

inline void LcLink(LcNode* p, LcNode* q) {
    LcEvert(q);
    if (LcRoot(p) != q)
        q->pa = p;
}

inline void LcCut(LcNode* p, LcNode* q) {
    LcEvert(p);
    LcExpose(q);
    SpSplay(q);
    if (q->ch[0] == p) {
        p->pa = nullptr;
        q->ch[0] = nullptr;
    }
}

inline uint32_t LcQry(LcNode* p, LcNode* q) {
    LcEvert(p);
    LcExpose(q);
    SpSplay(q);
    return q->sum;
}

LcNode T[MaxN];

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
            IoPut(LcQry(&T[u], &T[v]), '\n');
            break;
        case 1:
            LcLink(&T[u], &T[v]);
            break;
        case 2:
            LcCut(&T[u], &T[v]);
            break;
        case 3:
            SpMod(&T[u], v);
            break;
        }
    }
    return 0;
}
