#pragma once

#include "Common.hpp"
#include "ModArith.hpp"

namespace {
    template<int MaxExp_, Umx Mod_, Umx G_, class Uns_, class Tmp_>
    struct Ntt {
        typedef Uns_ Uns;
        typedef Tmp_ Tmp;
        static CEXV int MaxExp = MaxExp_;
        static CEXV Uns Mod = (Uns) Mod_;
        static CEXV Uns G = (Uns) G_;
        typedef ModArith<Mod, Uns, Tmp> MA;
        Uns WbFwd[MaxExp + 1];
        Uns WbInv[MaxExp + 1];
        Uns LenInv[MaxExp + 1];
        inline void Init(int Exp) NOEX {
            WbFwd[Exp] = MA::Pow(G, (Mod - 1) >> Exp);
            WbInv[Exp] = MA::Pow(MA::Inv(G), (Mod - 1) >> Exp);
            LenInv[Exp] = MA::Inv(MA::NUns(1u << Exp));
        }
        inline void Fwd(Uns A[], int Exp) {
            U32 Len = 1u << Exp;
            Uns Wn = WbFwd[Exp];
            for (int i = Exp - 1; i >= 0; --i) {
                U32 ChkSiz = 1u << i;
                for (U32 j = 0; j < Len; j += 2u << i) {
                    Uns W = 1;
                    for (U32 k = j; k < j + ChkSiz; ++k) {
                        Uns a = A[k];
                        Uns b = A[k + ChkSiz];
                        A[k] = MA::Add(a, b);
                        A[k + ChkSiz] = MA::Mul(W, MA::Sub(a, b));
                        W = MA::Mul(W, Wn);
                    }
                }
                Wn = MA::Mul(Wn, Wn);
            }
        }
        inline void Inv(Uns A[], int Exp) {
            U32 Len = 1u << Exp;
            Uns Ws[MaxExp];
            Ws[0] = WbInv[Exp];
            for (int i = 1; i < Exp; ++i)
                Ws[i] = MA::Mul(Ws[i - 1], Ws[i - 1]);
            for (int i = 0; i < Exp; ++i) {
                U32 ChkSiz = 1u << i;
                Uns Wn = Ws[Exp - 1 - i];
                for (U32 j = 0; j < Len; j += 2u << i) {
                    Uns W = 1;
                    for (U32 k = j; k < j + ChkSiz; ++k) {
                        Uns a = A[k];
                        Uns b = MA::Mul(W, A[k + ChkSiz]);
                        A[k] = MA::Add(a, b);
                        A[k + ChkSiz] = MA::Sub(a, b);
                        W = MA::Mul(W, Wn);
                    }
                }
            }
            for (U32 i = 0; i < Len; ++i)
                A[i] = MA::Mul(A[i], LenInv[Exp]);
        }
    };
}
