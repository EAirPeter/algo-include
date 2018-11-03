#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <vector>

using namespace std;

using Coord = double;
constexpr Coord Eps = 1e-8;

constexpr int Sgn(Coord val) noexcept {
    return val > Eps ? 1 : val < -Eps ? -1 : 0;
}

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
        return Sgn(AP % BP * CP.Len2() + BP % CP * AP.Len2() + CP % AP * BP.Len2()) > 0;
    }
    inline bool IsLeftOfVector(const Vec& A, const Vec& B) const noexcept {
        auto AB = B - A;
        auto AP = *this - A;
        return Sgn(AB % AP) > 0;
    }
};

inline Vec CircumCenter(const Vec& A, const Vec& B, const Vec& C) noexcept {
    auto AB = B - A;
    auto AC = C - A;
    auto lb = AB.Len2();
    auto lc = AC.Len2();
    auto den = (AB % AC) * 2;
    return A + Vec{(AC.y * lb - AB.y * lc) / den, (AB.x * lc - AC.x * lb) / den};
}

constexpr size_t DtMaxV = 100000;
constexpr size_t DtMaxE = DtMaxV * 6;
constexpr size_t DtMaxF = DtMaxV * 3;

struct DtVertex;
struct DtEdge;
struct DtFace;
using DtVPtr = DtVertex*;
using DtEPtr = DtEdge*;
using DtFPtr = DtFace*;

struct DtVertex : Vec {
    DtEPtr edge;
    Coord varea;
    using Vec::Vec;
} DtVtcs[DtMaxV];

struct DtEdge {
    DtVPtr orig;
    DtEPtr twin;
    DtEPtr prev;
    DtEPtr next;
    DtFPtr face;
    inline DtEdge& operator >>(DtEPtr rhs) noexcept {
        next = rhs;
        rhs->prev = this;
        return *rhs;
    }
} DtEBuf[DtMaxE];
DtEPtr DtEHead;

inline DtEPtr DtENew(DtVPtr A, DtVPtr B) noexcept {
    auto AB = DtEHead;
    auto BA = AB->next;
    DtEHead = BA->next;
    AB->orig = A;
    BA->orig = B;
    AB->twin = BA;
    BA->twin = AB;
    return AB;
}

inline void DtEDel(DtEPtr AB) noexcept {
    auto BA = AB->twin;
    AB->next = BA;
    BA->next = DtEHead;
    DtEHead = AB;
}

struct DtFace {
    DtEPtr edge;
    Vec circ;
} DtFBuf[DtMaxF];
size_t DtFCnt;

inline DtFPtr DtFNew(DtEPtr edge) noexcept {
    auto f = DtFBuf + DtFCnt++;
    f->edge = edge;
    auto e = edge;
    do {
        e->orig->edge = e;
        e->face = f;
        e = e->next;
    } while (e != edge);
    return f;
}

inline DtEPtr DtConnect(DtEPtr le, DtEPtr re) noexcept {
    auto LR = DtENew(le->orig, re->orig), RL = LR->twin;
    *le->prev >> LR;
    *re->prev >> RL;
    *LR >> re;
    *RL >> le;
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
        *fst->prev >> fst->twin->next;
        *snd >> base;
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
        *fst->prev >> fst->twin->next;
        *base >> snd->twin;
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
        auto AB = DtENew(A, B), BA = AB->twin;
        *AB >> BA >> AB;
        return AB;
    }
    if (size == 3) {
        auto A = &DtVtcs[beg + 0];
        auto B = &DtVtcs[beg + 1];
        auto C = &DtVtcs[beg + 2];
        auto AB = DtENew(A, B), BA = AB->twin;
        auto BC = DtENew(B, C), CB = BC->twin;
        auto det = Sgn((*B - *A) % (*C - *A));
        if (!det) {
            // Collinear
            *AB >> BC >> CB >> BA >> AB;
            return AB;
        }
        auto CA = DtENew(C, A), AC = CA->twin;
        *AB >> BC >> CA >> AB;
        *AC >> CB >> BA >> AC;
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

inline void DelaunayTriangulate(size_t N) noexcept {
    static DtFPtr Q[DtMaxF];
    if (N < 2)
        return;
    memset(DtEBuf, 0, sizeof(DtEBuf));
    DtEHead = DtEBuf;
    DtFCnt = 0;
    for (size_t i = 1; i < DtMaxE; ++i)
        DtEBuf[i - 1].next = &DtEBuf[i];
    sort(DtVtcs, DtVtcs + N);
    Q[0] = DtFNew(DtWork(0, N));
    for (size_t qbeg = 0, qend = 1; qbeg != qend; ++qbeg) {
        auto f = Q[qbeg];
        auto e = f->edge;
        do {
            if (!e->twin->face)
                Q[qend++] = DtFNew(e->twin);
            e = e->next;
        } while (e != f->edge);
    }
}

inline void VoronoiAreas(size_t N) noexcept {
    DelaunayTriangulate(N);
    for (size_t i = 1; i < DtFCnt; ++i) {
        auto AB = DtFBuf[i].edge;
        auto BC = AB->next;
        auto CA = BC->next;
        DtFBuf[i].circ = CircumCenter(*AB->orig, *BC->orig, *CA->orig);
    }
    for (size_t i = 0; i < N; ++i)
        DtVtcs[i].varea = 0;
    auto e = DtFBuf->edge;
    do {
        e->orig->varea = -1;
        e = e->next;
    } while (e != DtFBuf->edge);
    for (size_t i = 0; i < N; ++i)
        if (DtVtcs[i].varea != -1) {
            auto e = DtVtcs[i].edge->twin;
            auto& A = e->face->circ;
            e = e->twin->prev;
            auto AB = e->face->circ - A;
            e = e->twin->prev;
            while (e != DtVtcs[i].edge->twin) {
                auto AC = e->face->circ - A;
                DtVtcs[i].varea += AB % AC;
                AB = AC;
                e = e->twin->prev;
            }
            DtVtcs[i].varea /= 2;
        }
}

void DtVisualize(size_t N) {
    auto F = fopen("X.py", "w");
    fprintf(F, R"???(
from matplotlib.patches import Circle, Polygon
import matplotlib.pyplot as plt
import numpy as np

plt.figure(figsize=(16, 12), dpi=100)
ax = plt.axes()
)???");
    for (size_t i = 0; i < N; ++i)
        fprintf(F, "ax.add_patch(Circle(xy=(%f,%f), radius=0.1, fill=False))\n", DtVtcs[i].x, DtVtcs[i].y);
    for (size_t i = 0; i < DtFCnt; ++i) {
        auto e = DtFBuf[i].edge;
        fprintf(F, "ax.add_patch(Polygon(xy=np.array([[%f,%f]", e->orig->x, e->orig->y);
        e = e->next;
        while (e != DtFBuf[i].edge) {
            fprintf(F, ",[%f,%f]", e->orig->x, e->orig->y);
            e = e->next;
        }
        fprintf(F, "]), closed=True, fill=False))\n");
    }
    fprintf(F, R"???(
plt.axis('scaled')
ax.set_aspect('equal')
plt.savefig('X.png')
)???");
    fclose(F);
    system("python X.py");
    system("X.png");
}

void VoVisualize(size_t N) {
    auto F = fopen("Y.py", "w");
    fprintf(F, R"???(
from matplotlib.patches import Circle, Polygon
import matplotlib.pyplot as plt
import numpy as np

plt.figure(figsize=(16, 12), dpi=100)
ax = plt.axes()
)???");
    for (size_t i = 0; i < N; ++i)
        fprintf(F, "ax.add_patch(Circle(xy=(%f,%f), radius=0.1, fill=False))\n", DtVtcs[i].x, DtVtcs[i].y);
    for (size_t i = 0; i < N; ++i) {
        if (DtVtcs[i].varea != -1) {
            auto e = DtVtcs[i].edge->twin;
            fprintf(F, "ax.add_patch(Polygon(xy=np.array([[%f,%f]", e->face->circ.x, e->face->circ.y);
            e = e->twin->prev;
            while (e != DtVtcs[i].edge->twin) {
                fprintf(F, ",[%f,%f]", e->face->circ.x, e->face->circ.y);
                e = e->twin->prev;
            }
            fprintf(F, "]), closed=True, fill=False))\n");
        }
    }
    fprintf(F, R"???(
plt.axis('scaled')
ax.set_aspect('equal')
plt.savefig('Y.png')
)???");
    fclose(F);
    system("python Y.py");
    system("Y.png");
}
