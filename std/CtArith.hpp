#pragma once

#include "Common.hpp"

namespace { namespace CtArith {
    template<Umx N>
    struct ImplCtLog2 {
        static CEXV int res = ImplCtLog2<(N >> 1)>::res + 1;
    };
    template<> struct ImplCtLog2<1u> { static CEXV int res = 0; };
#define CtLog2(n_) (::CtArith::ImplCtLog2<(n_)>::res)
#define CtLog2C(n_) (CtLog2(((n_) << 1) - 1))
    template<Umx Mod, Umx A, Umx B>
    struct ImplCtPow {
        static CEXV Umx tmp = ImplCtPow<Mod, ImplCtPow<Mod, A, (B >> 1)>::res, 2>::res;
        static CEXV Umx res = B & 1 ? tmp * A % Mod : tmp;
    };
    template<Umx Mod, Umx A> struct ImplCtPow<Mod, A, 2u> { static CEXV Umx res = A * A % Mod; };
    template<Umx Mod, Umx A> struct ImplCtPow<Mod, A, 1u> { static CEXV Umx res = A % Mod; };
    template<Umx Mod, Umx A> struct ImplCtPow<Mod, A, 0u> { static CEXV Umx res = 1u; };
#define CtPow(mod_, a_, b_) (::CtArith::ImplCtPow<(mod_), (a_), (b_)>::res)
}}
