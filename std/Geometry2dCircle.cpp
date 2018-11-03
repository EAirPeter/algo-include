#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <utility>
#include <vector>

using namespace std;

const double EPS = 1e-12;
const double PI = acos(-1.0);

inline double sqr(double x) {
    return x * x;
}

inline int sgn(double x) {
    return fabs(x) < EPS ? 0 : x < 0 ? -1 : 1;
}

// Geometry2dCircle

inline double deg(double rad) {
    return rad / PI * 180;
}

inline double rdb() {
    double x;
    scanf("%lf", &x);
    return x;
}

inline void pdb(double x) {
    int c = sgn(x);
    if (!c)
        fputs("0.000000", stdout);
    else
        printf("%.6f", x);
}

struct pnt {
    double x, y;
    pnt() {
    }
    pnt(double x_, double y_) : x(x_), y(y_) {
    }
    pnt(double t_) : x(cos(t_)), y(sin(t_)) {
    }
    pnt(int) {
        scanf("%lf%lf", &x, &y);
    }
    inline bool operator <(const pnt &r) const {
        if (x != r.x)
            return x < r.x;
        return y < r.y;
    }
    inline pnt operator +(const pnt &r) const {
        return pnt(x + r.x, y + r.y);
    }
    inline pnt operator -(const pnt &r) const {
        return pnt(x - r.x, y - r.y);
    }
    inline pnt operator *(double r) const {
        return pnt(x * r, y * r);
    }
    inline pnt operator /(double r) const {
        return pnt(x / r, y / r);
    }
    inline double operator *(const pnt &r) const {
        return x * r.x + y * r.y;
    }
    inline double operator %(const pnt &r) const {
        return x * r.y - y * r.x;
    }
    inline double d2() const {
        return *this * *this;
    }
    inline double d() const {
        return sqrt(d2());
    }
    inline double t() const {
        return atan2(y, x);
    }
    inline double tl() const {
        double t_ = t();
        return t_ < 0 ? t_ + PI : t_;
    }
    inline pnt uni() const {
        return *this / d();
    }
    inline pnt prp() const {
        return pnt(y, -x);
    }
};

inline void ptup3(double x, double y, double z) {
    putchar('(');
    pdb(x);
    putchar(',');
    pdb(y);
    putchar(',');
    pdb(z);
    putchar(')');
    putchar('\n');
}

inline void plpnt(vector<pnt> &v) {
    sort(v.begin(), v.end());
    putchar('[');
    bool f = true;
    for (vector<pnt>::iterator i = v.begin(); i != v.end(); ++i) {
        if (f)
            f = false;
        else
            putchar(',');
        putchar('(');
        pdb(i->x);
        putchar(',');
        pdb(i->y);
        putchar(')');
    }
    putchar(']');
    putchar('\n');
}

inline void pldbl(vector<double> &v) {
    sort(v.begin(), v.end());
    putchar('[');
    bool f = true;
    for (vector<double>::iterator i = v.begin(); i != v.end(); ++i) {
        if (f)
            f = false;
        else
            putchar(',');
        pdb(*i);
    }
    putchar(']');
    putchar('\n');
}

inline pnt equ(double a1, double b1, double c1, double a2, double b2, double c2)
{
    double d = a1 * b2 - a2 * b1;
    return pnt((b2 * c1 - b1 * c2) / d, (c2 * a1 - c1 * a2) / d);
}

inline double di2(const pnt &A, const pnt &B) {
    return (A - B).d2();
}

inline double dis(const pnt &A, const pnt &B) {
    return (A - B).d();
}

inline pnt bis(const pnt &a, const pnt &b) {
    return pnt((a.t() + b.t()) / 2);
}

inline pnt mid(const pnt &A, const pnt &B) {
    return (A + B) / 2;
}

inline double dis(const pnt &P, const pnt &L, const pnt &l) {
    return fabs(l % (P - L)) / l.d();
}

inline double dis(const pnt &P, double a, double b, double c) {
    return fabs(a * P.x + b * P.y - c) / pnt(a, b).d();
}

inline pnt its(const pnt &A, const pnt &a, const pnt &B, const pnt &b) {
    double m = equ(a.x, -b.x, B.x - A.x, a.y, -b.y, B.y - A.y).x;
    return A + a * m;
}

inline vector<pnt> its(const pnt &O, double r, const pnt &P, const pnt &p) {
    pnt Q = its(O, p.prp(), P, p);
    double d = (O - Q).d();
    int c = sgn(r - d);
    if (c < 0)
        return vector<pnt>();
    if (!c)
        return vector<pnt>(1, Q);
    pnt AB = p.uni();
    double x = sqrt(sqr(r) - sqr(d));
    vector<pnt> v;
    v.push_back(Q + AB * x);
    v.push_back(Q - AB * x);
    return v;
}

inline vector<pnt> its(const pnt &O1, double r1, const pnt &O2, double r2) {
    int c = sgn(dis(O1, O2) - fabs(r1 - r2));
    if (c < 0)
        return vector<pnt>();
    if (!c)
        return vector<pnt>(1, O1 + (O2 - O1).uni() * r1);
    pnt O1O2 = O2 - O1;
    double lmd = ((sqr(r1) - sqr(r2)) / O1O2.d2() + 1) / 2;
    return its(O1, r1, O1 + O1O2 * lmd, O1O2.prp());
}

void CircumscribedCircle() {
    pnt A(0);
    pnt B(0);
    pnt C(0);
    pnt D = mid(A, B);
    pnt E = mid(B, C);
    pnt DO = (A - B).prp();
    pnt EO = (B - C).prp();
    pnt O = its(D, DO, E, EO);
    double r = dis(O, C);
    ptup3(O.x, O.y, r);
}

void InscribedCircle() {
    pnt A(0);
    pnt B(0);
    pnt C(0);
    pnt AI = bis(C - A, B - A);
    pnt BI = bis(A - B, C - B);
    pnt I = its(A, AI, B, BI);
    double r = dis(I, A, B - A);
    ptup3(I.x, I.y, r);
}

void TangentLineThroughPoint() {
    pnt O(0);
    double r = rdb();
    pnt P(0);
    vector<pnt> u = its(mid(O, P), dis(O, P) / 2, O, r);
    vector<double> v;
    for (vector<pnt>::iterator i = u.begin(); i != u.end(); ++i)
        v.push_back(deg((*i - O).prp().tl()));
    pldbl(v);
}

void CircleThroughAPointAndTangentToALineWithRadius() {
    pnt P(0);
    pnt P1(0);
    pnt P2(0);
    double r = rdb();
    pnt AB = P2 - P1;
    int c = sgn((P - P1) % (P2 - P1));
    pnt P3 = P1 + AB.prp().uni() * r * c;
    if (!c)
        AB = AB.prp();
    vector<pnt> v = its(P, r, P3, AB);
    plpnt(v);
}

void CircleTangentToTwoLinesWithRadius() {
    pnt A1(0);
    pnt B1(0);
    pnt A2(0);
    pnt B2(0);
    double r = rdb();
    pnt OP = B1 - A1;
    pnt OQ = B2 - A2;
    pnt O = its(A1, OP, A2, OQ);
    pnt OI1 = bis(OP, OQ).uni();
    pnt OI2 = OI1.prp().uni();
    double lmd1 = (OP.d() * OI1.d()) * r / (OP % OI1);
    double lmd2 = (OP.d() * OI2.d()) * r / (OP % OI2);
    vector<pnt> v;
    v.push_back(O + OI1 * lmd1);
    v.push_back(O - OI1 * lmd1);
    v.push_back(O + OI2 * lmd2);
    v.push_back(O - OI2 * lmd2);
    plpnt(v);
}

void CircleTangentToTwoDisjointCirclesWithRadius() {
    pnt O1(0);
    double r1 = rdb();
    pnt O2(0);
    double r2 = rdb();
    double r = rdb();
    vector<pnt> v = its(O1, r1 + r, O2, r2 + r);
    plpnt(v);
}

int main() {
    static char S[64];
    while (scanf("%s", S) == 1) {
        if (!strcmp(S, "CircumscribedCircle"))
            CircumscribedCircle();
        else if (!strcmp(S, "InscribedCircle"))
            InscribedCircle();
        else if (!strcmp(S, "TangentLineThroughPoint"))
            TangentLineThroughPoint();
        else if (!strcmp(S, "CircleThroughAPointAndTangentToALineWithRadius"))
            CircleThroughAPointAndTangentToALineWithRadius();
        else if (!strcmp(S, "CircleTangentToTwoLinesWithRadius"))
            CircleTangentToTwoLinesWithRadius();
        else if (!strcmp(S, "CircleTangentToTwoDisjointCirclesWithRadius"))
            CircleTangentToTwoDisjointCirclesWithRadius();
        else
            break;
    }
    return 0;
}

