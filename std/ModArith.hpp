#pragma once

#include "Gcd.hpp"
#include "ImplSign.hpp"

namespace {
    struct MATmpLD {};
    template<Umx Mod, class Uns, class Sig>
    CEXF Uns ImplMANSig(Sig N) NOEX {
        return (Uns) (N < 0 ? N % (Sig) Mod + (Sig) Mod : N % (Sig) Mod);
    }
    template<Umx Mod, class Uns, class Tmp>
    struct ImplMAMul {
        static CEXF Uns Mul(Uns A, Uns B) NOEX {
            return (Uns) ((Tmp) A * B % Mod);
        }
    };
    template<Umx Mod, class Uns>
    struct ImplMAMul<Mod, Uns, MATmpLD> {
        typedef typename ImplSign<Uns>::Sig Sig;
        static CEXF Uns Mul(Uns A, Uns B) NOEX {
            return ImplMANSig<Mod, Uns, Sig>((Sig) (
                A * B - Mod * (Uns) ((long double) A / (long double) Mod * (long double) B)
            ));
        }
    };
    template<Umx Mod_, class Uns_, class Tmp_ = Uns_>
    struct ModArith {
        typedef Uns_ Uns;
        typedef Tmp_ Tmp;
        static CEXV Uns Mod = (Uns) Mod_;
        typedef typename ImplSign<Uns>::Sig Sig;
        template<class AUns>
        static CEXF Uns NUns(AUns N) NOEX {
            return (Uns) (N % (AUns) Mod);
        }
        template<class ASig>
        static CEXF Uns NSig(ASig N) NOEX {
            return ImplMANSig<Mod, Uns, ASig>(N);
        }
        static CEXF Uns Add(Uns A, Uns B) NOEX {
            return A + B < Mod ? A + B : A + B - Mod;
        }
        static CEXF Uns Sub(Uns A, Uns B) NOEX {
            return A < B ? A - B + Mod : A - B;
        }
        static CEXF Uns Mul(Uns A, Uns B) NOEX {
            return ImplMAMul<Mod, Uns, Tmp>::Mul(A, B);
        }
        template<class Int>
        static CVMF Uns Pow(Uns A, Int B) NOEX {
            Uns res = 1;
            while (B) {
                if (B & 1)
                    res = Mul(res, A);
                A = Mul(A, A);
                B >>= 1;
            }
            return res;
        }
        static inline Uns Inv(Uns N) NOEX {
            Sig x, y;
            GcdEx(N, Mod, x, y);
            return NSig(x);
        }
        static inline Uns Div(Uns A, Uns B) NOEX {
            return Mul(A, Inv(B));
        }
    };
}
