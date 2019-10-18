#pragma once

namespace {
    template<class> struct ImplSign;
#define DEFIS(int_) \
    template<> struct ImplSign<signed int_> { \
        typedef signed int_ Sig; \
        typedef unsigned int_ Uns; \
    }; \
    template<> struct ImplSign<unsigned int_> : ImplSign<signed int_> {};
    DEFIS(char);
    DEFIS(short);
    DEFIS(int);
    DEFIS(long);
    DEFIS(long long);
#undef DEFIS
}
