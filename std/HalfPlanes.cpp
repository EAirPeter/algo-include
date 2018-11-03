#include <algorithm>
#include <cmath>
#include <cstdio>

using namespace std;

const double EPS = 1e-15;
const double PI = acos(-1.0);

inline double sgn(double x) {
    return x < -EPS ? -1 : x > EPS ? 1 : 0;
}

struct pnt {
    double x, y;
    pnt() {
    }
    pnt(double x_, double y_) : x(x_), y(y_) {
    }
    pnt(double t_) : x(cos(t_)), y(sin(t_)) {
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
    inline double t() const {
        double t_ = atan2(y, x);
        return sgn(t_ + PI) ? t_ : PI;
    }
};

inline pnt its(const pnt &A, const pnt &a, const pnt &B, const pnt &b) {
    return A + a * (((B - A) % b) / (a % b));
}

// HalfPlanes

struct lne {
    pnt A, B;
    double t;
    inline bool operator <(const lne &r) const {
        return sgn(t - r.t) ? t < r.t : sgn((B - A) % (r.A - A)) < 0;
    }
    inline void b(const pnt &B_) {
        B = B_;
        t = (B - A).t();
    }
} E[2000];

inline pnt its(const lne &L1, const lne &L2) {
    return its(L1.A, L1.B - L1.A, L2.A, L2.B - L2.A);
}

inline bool in(const pnt &P, const lne &l) {
    return (l.B - l.A) % (P - l.A) > EPS;
}

int Q[2000];

int main() {
    int tt;
    scanf("%d", &tt);
    while (tt--) {
        int N;
        scanf("%d", &N);
        for (int i = 0; i < N; ++i)
            scanf("%lf%lf", &E[i].A.x, &E[i].A.y);
        double S1 = 0;
        for (int i = 2; i < N; ++i)
            S1 += (E[i - 1].A - E[0].A) % (E[i].A - E[0].A);
        if (S1 > 0) {
            for (int i = 0; i < N - 1; ++i)
                E[i].b(E[i + 1].A);
            E[N - 1].b(E[0].A);
        }
        else {
            for (int i = 1; i < N; ++i)
                E[i].b(E[i - 1].A);
            E[0].b(E[N - 1].A);
        }
        sort(E, E + N);
        int M = 1;
        for (int i = 1; i < N; ++i)
            if (sgn(E[i].t - E[M - 1].t))
                E[M++] = E[i];
        int qf = 0, ql = -1;
        Q[++ql] = 0;
        Q[++ql] = 1;
        for (int i = 2; i < M; ++i) {
            while (ql > qf && !in(its(E[Q[ql - 1]], E[Q[ql]]), E[i]))
                --ql;
            while (ql > qf && !in(its(E[Q[qf + 1]], E[Q[qf]]), E[i]))
                ++qf;
            Q[++ql] = i;
        }
        while (ql > qf && !in(its(E[Q[ql - 1]], E[Q[ql]]), E[Q[qf]]))
            --ql;
        while (ql > qf && !in(its(E[Q[qf + 1]], E[Q[qf]]), E[Q[ql]]))
            ++qf;
        pnt O = its(E[Q[qf]], E[Q[ql]]);
        double S2 = 0;
        for (int i = qf + 1; i < ql; ++i) {
            pnt A = its(E[Q[i]], E[Q[i - 1]]);
            pnt B = its(E[Q[i]], E[Q[i + 1]]);
            S2 += (A - O) % (B - O);
        }
        S2 = fabs(S2 / 2);
        if (S2 > EPS)
            printf("%.2f\n", S2);
        else
            puts("0.00");
    }
    return 0;
}

