#include <algorithm>
#include <cmath>
#include <cstdio>
#include <utility>

using namespace std;

const double EPS = 1e-9;

inline int sgn(double x) {
    return  x > EPS ? 1 : x < -EPS ? -1 : 0;
}

// Geometry3d

struct pnt {
    double x, y, z;
    pnt() {
    }
    pnt(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {
    }
    inline pnt operator +(const pnt &r) const {
        return pnt(x + r.x, y + r.y, z + r.z);
    }
    inline pnt operator -(const pnt &r) const {
        return pnt(x - r.x, y - r.y, z - r.z);
    }
    inline pnt operator *(double r) const {
        return pnt(x * r, y * r, z * r);
    }
    inline pnt operator /(double r) const {
        return pnt(x / r, y / r, z / r);
    }
    inline double operator *(const pnt &r) const {
        return x * r.x + y * r.y + z * r.z;
    }
    inline pnt operator %(const pnt &r) const {
        return pnt(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x);
    }
    inline double d2() const {
        return *this * *this;
    }
    inline double d() const {
        return sqrt(d2());
    }
    inline pnt uni() const {
        return *this / d();
    }
};

inline double det(double D11, double D12, double D13,
    double D21, double D22, double D23, double D31, double D32, double D33)
{
    return D11 * D22 * D33 + D12 * D23 * D31 + D13 * D21 * D32 -
        D11 * D32 * D23 - D12 * D33 * D21 - D13 * D31 * D22;
}

inline pnt its(const pnt &A, const pnt &a, const pnt &B, const pnt &b,
    const pnt &C, const pnt &c)
{
    double A1 = a.x;
    double B1 = a.y;
    double C1 = a.z;
    double D1 = A * a;
    double A2 = b.x;
    double B2 = b.y;
    double C2 = b.z;
    double D2 = B * b;
    double A3 = c.x;
    double B3 = c.y;
    double C3 = c.z;
    double D3 = C * c;
    double d0 = det(A1, B1, C1, A2, B2, C2, A3, B3, C3);
    double dx = det(D1, B1, C1, D2, B2, C2, D3, B3, C3);
    double dy = det(A1, D1, C1, A2, D2, C2, A3, D3, C3);
    double dz = det(A1, B1, D1, A2, B2, D2, A3, B3, D3);
    return pnt(dx / d0, dy / d0, dz / d0);
}

int main() {
    pnt A, B, C, D;
    while (scanf("%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf",
        &A.x, &A.y, &A.z,
        &B.x, &B.y, &B.z,
        &C.x, &C.y, &C.z,
        &D.x, &D.y, &D.z) == 12)
    {
        pnt NA = (B - A) % (C - A);
        int g = sgn(NA * (D - A));
        if (!g) {
            puts("O O O O");
            continue;
        }
        if (g < 0) {
            swap(A, B);
            NA = (B - A) % (C - A);
        }
        NA = NA.uni();
        pnt NB = ((D - B) % (C - B)).uni();
        pnt NC = ((D - C) % (A - C)).uni();
        pnt ND = ((B - D) % (A - D)).uni();
        double l = 0, r = (D - A).d();
        while (r - l > 1e-8) {
            double m = (l + r) / 2;
            pnt A1 = A + NA * m;
            pnt B1 = B + NB * m;
            pnt C1 = C + NC * m;
            pnt D1 = D + ND * m;
            pnt A2 = its(C1, NC, D1, ND, A1, NA);
            //pnt B2 = its(D1, ND, A1, NA, B1, NB);
            //pnt C2 = its(A1, NA, B1, NB, C1, NC);
            pnt D2 = its(B1, NB, C1, NC, D1, ND);
            if ((D2 - A2) * NA < 0)
                r = m;
            else
                l = m;
        }
        pnt A1 = A + NA * l;
        pnt B1 = B + NB * l;
        pnt C1 = C + NC * l;
        pnt D1 = D + ND * l;
        pnt A2 = its(C1, NC, D1, ND, A1, NA);
        pnt B2 = its(D1, ND, A1, NA, B1, NB);
        pnt C2 = its(A1, NA, B1, NB, C1, NC);
        pnt D2 = its(B1, NB, C1, NC, D1, ND);
        pnt I = (A2 + B2 + C2 + D2) / 4;
        printf("%.4f %.4f %.4f %.4f\n", I.x, I.y, I.z, l);
    }
    return 0;
}

