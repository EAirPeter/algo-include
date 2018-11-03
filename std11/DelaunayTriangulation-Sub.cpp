// LGOJ1265

#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <utility>

using namespace std;

using Coord = double;

struct Vec {
    Coord x, y;
    inline Vec() noexcept = default;
    constexpr Vec(Coord x_, Coord y_) noexcept : x(x_), y(y_) {}
    constexpr bool operator <(const Vec& rhs) const noexcept {
        return x < rhs.x ? true : x > rhs.x ? false : y < rhs.y;
    }
    constexpr Vec operator +(const Vec& rhs) const noexcept {
        return {x + rhs.x, y + rhs.y};
    }
    constexpr Vec operator -(const Vec& rhs) const noexcept {
        return {x - rhs.x, y - rhs.y};
    }
    constexpr Coord operator *(const Vec& rhs) const noexcept {
        return x * rhs.x + y * rhs.y;
    }
    constexpr Coord operator %(const Vec& rhs) const noexcept {
        return x * rhs.y - y * rhs.x;
    }
    constexpr Coord Len2() const noexcept {
        return *this * *this;
    }
    inline bool IsInCircumCircle(const Vec& A, const Vec& B, const Vec& C) const noexcept {
        auto AP = *this - A;
        auto BP = *this - B;
        auto CP = *this - C;
        return AP % BP * CP.Len2() + BP % CP * AP.Len2() + CP % AP * BP.Len2() > 0;
    }
    inline bool IsLeftOfVector(const Vec& A, const Vec& B) const noexcept {
        auto AB = B - A;
        auto AP = *this - A;
        return AB % AP > 0;
    }
};

constexpr size_t DtMaxV = 5000;
constexpr size_t DtMaxE = DtMaxV * 3;
constexpr size_t DtMaxF = DtMaxV * 3;

struct DtVertex;
struct DtEdge;
struct DtFace;

struct DtVertex : Vec {
    size_t edge;
    using Vec::Vec;
} DtVtcs[DtMaxV];

struct DtEdge {
    size_t orig, prev, next, face;
} DtEBuf[DtMaxE << 1];
size_t DtEHead;

inline void DtELink(size_t iAB, size_t iCD) noexcept {
    DtEBuf[iAB].next = iCD;
    DtEBuf[iCD].prev = iAB;
}

inline size_t DtENew(size_t iA, size_t iB) noexcept {
    auto iAB = DtEHead;
    DtEHead = DtEBuf[DtEHead].next;
    DtEBuf[iAB].orig = iA;
    DtEBuf[iAB ^ 1].orig = iB;
    return iAB;
}

inline void DtEDel(size_t iAB) noexcept {
    iAB &= ~1;
    DtEBuf[iAB].next = DtEHead;
    DtEHead = iAB;
}

struct DtFace {
    size_t edge;
} DtFBuf[DtMaxF];
size_t DtFCnt;

inline size_t DtFNew(size_t edge) noexcept {
    auto& f = DtFBuf[DtFCnt];
    f.edge = edge;
    DtVtcs[DtFBuf[edge].orig].edge = edge;
    for (auto ie = DtEBuf[edge].next; ie != edge; ie = DtEBuf[ie].next) {
        DtEBuf[ie].orig
        e->orig->edge = e;
        e->face = f;
        e = e->next;
    } while (e != edge);
    return f;
}

inline DtEPtr DtConnect(DtEPtr le, DtEPtr re) noexcept {
    auto LR = DtENew(le->orig);
    auto RL = DtENew(re->orig);
    DtEBind(LR, RL);
    DtELink(le->prev, LR);
    DtELink(re->prev, RL);
    DtELink(LR, re);
    DtELink(RL, le);
    return LR;
}

inline DtEPtr DtTangent(DtEPtr le, DtEPtr re) noexcept {
    for (auto e = le->next; e != le; e = e->next)
        if (*le->orig < *e->orig)
            le = e;
    for (auto e = re->next; e != re; e = e->next)
        if (*e->orig < *re->orig)
            re = e;
    DtEPtr xle, xre;
    do {
        xle = le;
        xre = re;
        while (le->next->orig->IsLeftOfVector(*re->orig, *le->orig))
            le = le->next;
        while (re->prev->orig->IsLeftOfVector(*re->orig, *le->orig))
            re = re->prev;
    } while (le != xle || re != xre);
    return DtConnect(le, re);
}

inline bool DtFindLeft(DtEPtr base) noexcept {
    auto L = base->orig;
    auto R = base->twin->orig;
    auto fst = base->prev;
    if (!fst->orig->IsLeftOfVector(*L, *R))
        return false;
    auto snd = fst->twin->prev;
    while (snd != base->twin && snd->orig->IsInCircumCircle(*L, *R, *fst->orig)) {
        DtELink(fst->prev, fst->twin->next);
        DtELink(snd, base);
        DtEDel(fst);
        fst = snd;
        snd = fst->twin->prev;
    }
    return true;
}

inline bool DtFindRight(DtEPtr base) noexcept {
    auto L = base->orig;
    auto R = base->twin->orig;
    auto fst = base->next->twin;
    if (!fst->orig->IsLeftOfVector(*L, *R))
        return false;
    auto snd = fst->next->twin;
    while (snd != base && snd->orig->IsInCircumCircle(*L, *R, *fst->orig)) {
        DtELink(fst->prev, fst->twin->next);
        DtELink(base, snd->twin);
        DtEDel(fst);
        fst = snd;
        snd = fst->next->twin;
    }
    return true;
}

// Divide and Conquer
DtEPtr DtWork(size_t beg, size_t end) noexcept {
    auto size = end - beg;
    // size >= 2
    if (size == 2) {
        auto A = &DtVtcs[beg + 0];
        auto B = &DtVtcs[beg + 1];
        auto AB = DtENew(A);
        auto BA = DtENew(B);
        DtEBind(AB, BA);
        DtELink(AB, BA);
        DtELink(BA, AB);
        return AB;
    }
    if (size == 3) {
        auto A = &DtVtcs[beg + 0];
        auto B = &DtVtcs[beg + 1];
        auto C = &DtVtcs[beg + 2];
        auto AB = DtENew(A);
        auto BA = DtENew(B);
        DtEBind(AB, BA);
        auto BC = DtENew(B);
        auto CB = DtENew(C);
        DtEBind(BC, CB);
        auto det = (*B - *A) % (*C - *A);
        if (!det) {
            // Collinear
            DtELink(AB, BC);
            DtELink(BC, CB);
            DtELink(CB, BA);
            DtELink(BA, AB);
            return AB;
        }
        auto CA = DtENew(C);
        auto AC = DtENew(A);
        DtEBind(CA, AC);
        DtELink(AB, BC);
        DtELink(BC, CA);
        DtELink(CA, AB);
        DtELink(AC, CB);
        DtELink(CB, BA);
        DtELink(BA, AC);
        return det > 0 ? AC : CA;
    }
    auto mid = beg + size / 2;
    auto base = DtTangent(DtWork(beg, mid), DtWork(mid, end));
    auto flag = (int) DtFindLeft(base) << 1 | (int) DtFindRight(base);
    while (flag) {
        auto L = base->orig;
        auto R = base->twin->orig;
        auto le = base->prev;
        auto re = base->next->next;
        if (flag == 3)
            flag = le->orig->IsInCircumCircle(*L, *R, *re->orig) ? 2 : 1;
        base = flag == 2 ? DtConnect(le, base->next) : DtConnect(base, re);
        flag = (int) DtFindLeft(base) << 1 | (int) DtFindRight(base);
    }
    return base;
}

inline DtFPtr DtBuildFace(DtEPtr e) noexcept {
    static DtFPtr Q[DtMaxF];
    Q[0] = DtFNew(e);
    for (size_t qbeg = 0, qend = 1; qbeg != qend; ++qbeg) {
        auto f = Q[qbeg];
        auto e = f->edge;
        do {
            if (!e->twin->face)
                Q[qend++] = DtFNew(e->twin);
            e = e->next;
        } while (e != f->edge);
    }
    return e->face;
}

inline DtFPtr DelaunayTriangulate(size_t N) noexcept {
    if (N < 2)
        return nullptr;
    memset(DtEBuf, 0, sizeof(DtEBuf));
    DtEHead = DtEBuf;
    DtFCnt = 0;
    for (size_t i = 1; i < DtMaxE; ++i)
        DtEBuf[i - 1].next = &DtEBuf[i];
    sort(DtVtcs, DtVtcs + N);
    return DtBuildFace(DtWork(0, N));
}

struct GVertex;
struct GEdge;
using GVPtr = GVertex*;
using GEPtr = GEdge*;

struct GVertex {
    GVPtr ufsParent = nullptr;
    size_t ufsSize = 1;
    GVPtr UfsRoot() noexcept {
        return ufsParent ? ufsParent = ufsParent->UfsRoot() : this;
    }
    friend inline void UfsUnion(GVPtr lhs, GVPtr rhs) noexcept {
        if (lhs->ufsSize < rhs->ufsSize)
            swap(lhs, rhs);
        rhs->ufsParent = lhs;
        lhs->ufsSize += rhs->ufsSize;
    }
} GVtcs[DtMaxV];

struct GEdge {
    GVPtr U, V;
    double weight;
    constexpr bool operator <(const GEdge& rhs) const noexcept {
        return weight < rhs.weight;
    }
} GEBuf[DtMaxE];
size_t GECnt;

inline void GENew(DtEPtr e) noexcept {
    auto c = GEBuf + GECnt++;
    auto U = e->orig;
    auto V = e->twin->orig;
    c->U = &GVtcs[U - DtVtcs];
    c->V = &GVtcs[V - DtVtcs];
    c->weight = sqrt((*V - *U).Len2());
}

int main() {
    size_t N;
    scanf("%zu", &N);
    for (size_t i = 0; i < N; ++i)
        scanf("%lf%lf", &DtVtcs[i].x, &DtVtcs[i].y);
    DelaunayTriangulate(N);
    for (size_t i = 0; i < DtFCnt; ++i) {
        auto e = DtFBuf[i].edge;
        do {
            if (e->twin) {
                GENew(e);
                e->twin->twin = nullptr;
            }
            e = e->next;
        } while (e != DtFBuf[i].edge);
    }
    sort(GEBuf, GEBuf + GECnt);
    double res = 0;
    for (size_t i = 0; i < GECnt; ++i) {
        auto e = &GEBuf[i];
        auto U = e->U->UfsRoot();
        auto V = e->V->UfsRoot();
        if (U != V) {
            res += e->weight;
            UfsUnion(U, V);
        }
    }
    printf("%.2f\n", res);
    return 0;
}
