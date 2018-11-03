#include <cmath>
#include <cstdio>

using namespace std;

const double EPS = 1e-8;

// Geometry2d
// Euler: V + F - E = 2

inline double sgn(double x) {
    return x > EPS ? 1 : x < -EPS ? -1 : 0;
}

struct pt {
    double x, y;
    pt() {
    }
    pt(double x_, double y_) : x(x_), y(y_) {
    }
    pt(int) {
        scanf("%lf%lf", &x, &y);
    }
    inline bool operator ==(const pt &r) const {
        return !sgn(x - r.x) && !sgn(y - r.y);
    }
    inline pt operator +(const pt &r) const {
        return pt(x + r.x, y + r.y);
    }
    inline pt operator -(const pt &r) const {
        return pt(x - r.x, y - r.y);
    }
    inline pt operator *(double r) const {
        return pt(x * r, y * r);
    }
    inline pt operator /(double r) const {
        return pt(x / r, y / r);
    }
    inline double operator *(const pt &r) const {
        return x * r.x + y * r.y;
    }
    inline double operator %(const pt &r) const {
        return x * r.y - y * r.x;
    }
    inline double d2() const {
        return *this * *this;
    }
    inline double d() const {
        return sqrt(d2());
    }
    inline void uni() {
        double d_ = d();
        x /= d_;
        y /= d_;
    }
    inline pt prp() const {
        return pt(y, -x);
    }
};

inline pt its(const pt &A, const pt &a, const pt &B, const pt &b) {
    return A + a * (((B - A) % b) / (a % b));
}

bool Main() {
    pt O(0);
    double r;
    scanf("%lf", &r);
    pt A(0);
    pt v(0);
    pt B(0);
    v.uni();
    double d = fabs(v % (O - A));
    if (v * (O - A) < EPS || d > r - EPS)
        return !sgn(v % (B - A)) && v * (B - A) > -EPS;
    if (d < EPS)
        return !sgn((A - O) % (B - O)) && (A - O) * (B - O) > -EPS;
    pt P = its(A, v, O, v.prp()) - v * sqrt(r * r - d * d);
    if (!sgn((P - B) % (A - B)) && (P - B) * (A - B) < EPS)
        return true;
    pt OP = P - O;
    pt Q = its(A, OP.prp(), O, OP);
    pt C = A + (Q - A) * 2;
    return !sgn((C - P) % (B - P)) && (C - P) * (B - P) > -EPS;
}

int main() {
    int tt;
    scanf("%d", &tt);
    for (int nn = 1; nn <= tt; ++nn)
        printf(Main() ? "Case #%d: Yes\n" : "Case #%d: No\n", nn);
    return 0;
}

