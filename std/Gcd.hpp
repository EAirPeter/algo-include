#pragma once

#include "Common.hpp"
#include "ImplSign.hpp"

namespace {
    template<class Uns>
    CVMF Uns Gcd(Uns A, Uns B) NOEX {
        while (B) {
            Uns C = A % B;
            A = B;
            B = C;
        }
        return A;
    }
    template<class Uns, class Sig>
    CVMF Uns GcdEx(Uns A, Uns B, Sig& x, Sig& y) NOEX {
        if (!B) {
            x = 1;
            y = 0;
            return A;
        }
        Uns d = GcdEx(B, A % B, y, x);
        y -= x * (Sig) (A / B);
        return d;
    }
}
